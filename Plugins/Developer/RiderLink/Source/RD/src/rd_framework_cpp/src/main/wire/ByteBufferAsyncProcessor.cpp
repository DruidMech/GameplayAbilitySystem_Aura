#include "ByteBufferAsyncProcessor.h"

#include "util/guards.h"
#include <util/thread_util.h>

#include "spdlog/sinks/stdout_color_sinks.h"

namespace rd
{
size_t ByteBufferAsyncProcessor::INITIAL_CAPACITY = 1024 * 1024;

std::shared_ptr<spdlog::logger> ByteBufferAsyncProcessor::logger =
	spdlog::stderr_color_mt<spdlog::synchronous_factory>("byteBufferLog", spdlog::color_mode::automatic);

ByteBufferAsyncProcessor::ByteBufferAsyncProcessor(
	std::string id, std::function<bool(Buffer::ByteArray const&, sequence_number_t)> processor)
	: id(std::move(id)), processor(std::move(processor))
{
	data.reserve(INITIAL_CAPACITY);
}

void ByteBufferAsyncProcessor::cleanup0()
{
	{
		std::lock_guard<decltype(lock)> guard(lock);

		state = StateKind::Terminated;
	}
	// TO-DO clean data

	cv.notify_all();
}

bool ByteBufferAsyncProcessor::terminate0(time_t timeout, StateKind state_to_set, string_view action)
{
	{
		std::lock_guard<decltype(lock)> guard(lock);
		if (state == StateKind::Initialized)
		{
			logger->debug("Can't {} \'{}\', because it hasn't been started yet", std::string(action), id);
			cleanup0();
			return true;
		}

		if (state >= state_to_set)
		{
			logger->debug("Trying to {} async processor \'{}' but it's in state {}", std::string(action), id, to_string(state));
			return true;
		}

		state = state_to_set;
	}
	cv.notify_all();

	std::future_status status = async_future.wait_for(timeout);

	bool success = true;

	if (status == std::future_status::timeout)
	{
		logger->error("Couldn't wait async thread during time: {}", to_string(timeout));
		success = false;
	}

	cleanup0();

	return success;
}

void ByteBufferAsyncProcessor::add_data(std::vector<Buffer::ByteArray>&& new_data)
{
	std::lock_guard<decltype(queue_lock)> guard(queue_lock);
	std::move(new_data.begin(), new_data.end(), std::back_inserter(queue));
	//		for (auto &&item : new_data) {
	//			queue.emplace(std::move(item));
	//		}
}

bool ByteBufferAsyncProcessor::reprocess()
{
	{
		std::lock_guard<decltype(queue_lock)> guard(queue_lock);

		logger->debug("{}: reprocessing started", id);

		std::unique_lock<decltype(processing_lock)> ul(processing_lock);
		processing_cv.wait(ul, [this]() -> bool { return !in_processing; });

		logger->debug("{}: reprocessing waited for main processing", id);

		while (current_seqn <= acknowledged_seqn)
		{
			pending_queue.pop_front();
			++current_seqn;
		}
		for (int i = 0; i < pending_queue.size(); ++i)
		{
			auto const& item = pending_queue[i];
			if (!processor(item, current_seqn + i))
			{
				return false;
			}
		}
	}
	return true;
}

void ByteBufferAsyncProcessor::process()
{
	{
		std::lock_guard<decltype(queue_lock)> guard(queue_lock);
		std::unique_lock<decltype(processing_lock)> ul(processing_lock);
		util::bool_guard bool_guard(in_processing);

		logger->debug("{}: processing started", id);

		while (!queue.empty() && processor(queue.front(), max_sent_seqn + 1))
		{
			++max_sent_seqn;
			pending_queue.push_back(std::move(queue.front()));
			queue.pop_front();
		}
	}
	processing_cv.notify_all();

	cv.notify_all();
}

void ByteBufferAsyncProcessor::ThreadProc()
{
	rd::util::set_thread_name(id.empty() ? "ByteBufferAsyncProcessor Thread" : id.c_str());
	async_thread_id = std::this_thread::get_id();

	while (true)
	{
		{
			std::lock_guard<decltype(lock)> guard(lock);

			if (state >= StateKind::Terminated)
			{
				return;
			}

			while (data.empty() || interrupt_balance != 0)
			{
				if (state >= StateKind::Stopping)
				{
					return;
				}
				cv.wait(lock);

				logger->debug("{}'s ThreadProc waited for notify", id);

				if (state >= StateKind::Terminating)
				{
					return;
				}
			}
			add_data(std::move(data));
			data.clear();
		}

		try
		{
			process();
		}
		catch (std::exception const& e)
		{
			logger->error("Exception while processing byte queue | {}", e.what());
		}
	}
}

void ByteBufferAsyncProcessor::start()
{
	{
		std::lock_guard<decltype(lock)> guard(lock);

		if (state != StateKind::Initialized)
		{
			logger->debug("Trying to START async processor {} but it's in state {}", id, to_string(state));
			return;
		}

		state = StateKind::AsyncProcessing;

		async_future = std::async(std::launch::async, &ByteBufferAsyncProcessor::ThreadProc, this);
	}
}

bool ByteBufferAsyncProcessor::stop(time_t timeout)
{
	return terminate0(timeout, StateKind::Stopping, "STOP");
}

bool ByteBufferAsyncProcessor::terminate(time_t timeout)
{
	return terminate0(timeout, StateKind::Terminating, "TERMINATE");
}

void ByteBufferAsyncProcessor::put(Buffer::ByteArray new_data)
{
	{
		std::lock_guard<decltype(lock)> guard(lock);

		if (state >= StateKind::Stopping)
		{
			return;
		}
		data.emplace_back(std::move(new_data));
	}
	cv.notify_all();
}

void ByteBufferAsyncProcessor::pause(const std::string& reason)
{
	std::lock_guard<decltype(lock)> guard(lock);

	++interrupt_balance;

	logger->debug("{} paused with reason={},state={}", id, reason, to_string(state));

	auto current_thread_id = std::this_thread::get_id();
	if (current_thread_id != async_thread_id)
	{
		logger->debug("{} paused from another thread : {}", id, to_string(current_thread_id));
		std::unique_lock<decltype(processing_lock)> ul(processing_lock);
		processing_cv.wait(ul, [this]() -> bool { return !in_processing; });
		logger->debug("{}: pausing waited for main processing", id);
	}
}

void ByteBufferAsyncProcessor::resume()
{
	{
		std::lock_guard<decltype(lock)> guard(lock);

		reprocess();

		--interrupt_balance;

		logger->debug("{} resumed", id);
	}

	cv.notify_all();
}

void ByteBufferAsyncProcessor::acknowledge(sequence_number_t seqn)
{
	std::lock_guard<decltype(lock)> guard(lock);

	if (seqn > acknowledged_seqn)
	{
		logger->trace("{}: new acknowledged seqn: {}", this->id, seqn);
		acknowledged_seqn = seqn;
	}
	else
	{
		logger->error("Acknowledge {} called, while next seqn MUST BE greater than {}", seqn, acknowledged_seqn);
	}
}

std::string to_string(ByteBufferAsyncProcessor::StateKind state)
{
	switch (state)
	{
		case ByteBufferAsyncProcessor::StateKind::Initialized:
			return "Initialized";
		case ByteBufferAsyncProcessor::StateKind::AsyncProcessing:
			return "AsyncProcessing";
		case ByteBufferAsyncProcessor::StateKind::Stopping:
			return "Stopping";
		case ByteBufferAsyncProcessor::StateKind::Terminating:
			return "Terminating";
		case ByteBufferAsyncProcessor::StateKind::Terminated:
			return "Terminated";
	}
	return {};
}
}	 // namespace rd
