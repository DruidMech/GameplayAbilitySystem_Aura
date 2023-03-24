#ifndef RD_CPP_BYTEBUFFERASYNCPROCESSOR_H
#define RD_CPP_BYTEBUFFERASYNCPROCESSOR_H

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable:4251)
#endif

#include "protocol/Buffer.h"
#include "spdlog/spdlog.h"

#include <chrono>
#include <string>
#include <mutex>
#include <condition_variable>
#include <future>
#include <list>

#include <rd_framework_export.h>

namespace rd
{
using sequence_number_t = int64_t;

class RD_FRAMEWORK_API ByteBufferAsyncProcessor
{
public:
	enum class StateKind
	{
		Initialized,
		AsyncProcessing,
		Stopping,
		Terminating,
		Terminated
	};

private:
	using time_t = std::chrono::milliseconds;

	static size_t INITIAL_CAPACITY;

	std::recursive_mutex lock;
	std::condition_variable_any cv;

	std::string id;

	std::function<bool(Buffer::ByteArray const&, sequence_number_t seqn)> processor;

	StateKind state{StateKind::Initialized};
	static std::shared_ptr<spdlog::logger> logger;

	std::thread::id async_thread_id;
	std::future<void> async_future;

	std::vector<Buffer::ByteArray> data;
	std::mutex queue_lock;
	std::deque<Buffer::ByteArray> queue{};
	std::deque<Buffer::ByteArray> pending_queue{};

	sequence_number_t max_sent_seqn = 0;
	sequence_number_t current_seqn = 1;
	sequence_number_t acknowledged_seqn = 0;

	int32_t interrupt_balance = 0;
	bool in_processing = false;
	std::mutex processing_lock;
	std::condition_variable processing_cv;

public:
	// region ctor/dtor

	explicit ByteBufferAsyncProcessor(std::string id, std::function<bool(Buffer::ByteArray const&, sequence_number_t)> processor);

	// endregion
private:
	void cleanup0();

	bool terminate0(time_t timeout, StateKind state_to_set, string_view action);

	void add_data(std::vector<Buffer::ByteArray>&& new_data);

	bool reprocess();

	void process();

	void ThreadProc();

public:
	void start();

	bool stop(time_t timeout = time_t(0));

	bool terminate(time_t timeout = time_t(0) /*InfiniteDuration*/);

	void put(Buffer::ByteArray new_data);

	void pause(const std::string& reason);

	void resume();

	void acknowledge(int64_t seqn);
};

std::string to_string(ByteBufferAsyncProcessor::StateKind state);
}	 // namespace rd
#if defined(_MSC_VER)
#pragma warning(pop)
#endif


#endif	  // RD_CPP_BYTEBUFFERASYNCPROCESSOR_H
