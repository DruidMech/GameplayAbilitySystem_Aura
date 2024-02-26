#ifndef RD_CPP_SOCKETWIRE_H
#define RD_CPP_SOCKETWIRE_H

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable:4251)
#endif

#include "scheduler/base/IScheduler.h"
#include "base/WireBase.h"
#include "ByteBufferAsyncProcessor.h"
#include "PkgInputStream.h"

#include <string>
#include <array>
#include <condition_variable>

#include <rd_framework_export.h>

class CSimpleSocket;
class CActiveSocket;
class CPassiveSocket;

namespace rd
{
class RD_FRAMEWORK_API SocketWire
{
	static std::chrono::milliseconds timeout;

public:
	class RD_FRAMEWORK_API Base : public WireBase
	{
	protected:
		static std::shared_ptr<spdlog::logger> logger;

		std::timed_mutex lock;
		mutable std::mutex socket_send_lock;
		mutable std::mutex wire_send_lock;

		std::thread thread{};

		std::string id;
		IScheduler* scheduler = nullptr;
		std::shared_ptr<CSimpleSocket> socket_provider;

		std::shared_ptr<CActiveSocket> socket;

		mutable std::condition_variable socket_send_var;
		mutable ByteBufferAsyncProcessor async_send_buffer{id + "-AsyncSendProcessor",
			[this](Buffer::ByteArray const& it, sequence_number_t seqn) -> bool { return this->send0(it, seqn); }};

		static constexpr size_t RECEIVE_BUFFER_SIZE = 1u << 16;
		mutable std::array<Buffer::word_t, RECEIVE_BUFFER_SIZE> receiver_buffer{};
		mutable decltype(receiver_buffer)::iterator lo = receiver_buffer.begin(), hi = receiver_buffer.begin();

		static constexpr int32_t ACK_MESSAGE_LENGTH = -1;
		static constexpr int32_t PING_MESSAGE_LENGTH = -2;
		static constexpr int32_t PACKAGE_HEADER_LENGTH = sizeof(ACK_MESSAGE_LENGTH) + sizeof(sequence_number_t);
		mutable Buffer ack_buffer{PACKAGE_HEADER_LENGTH};

		/**
		 * \brief Timestamp of this wire which increases at intervals of [heartBeatInterval].
		 */
		mutable int32_t current_timestamp = 0;

		/**
		 * \brief Actual knowledge about counterpart's [currentTimeStamp].
		 */
		mutable int32_t counterpart_timestamp = 0;

		/**
		 * \brief The latest received counterpart's acknowledge of this wire's [currentTimeStamp].
		 */
		mutable int32_t counterpart_acknowledge_timestamp = 0;

		mutable Buffer ping_pkg_header{PACKAGE_HEADER_LENGTH};

		mutable sequence_number_t max_received_seqn = 0;
		mutable Buffer send_package_header{PACKAGE_HEADER_LENGTH};

		static constexpr int32_t CHUNK_SIZE = 16370;
		mutable int32_t sz = -1;
		mutable RdId::hash_t id_ = -1;
		mutable PkgInputStream receive_pkg{[this]() -> int32_t { return this->read_package(); }};

		mutable Buffer message{CHUNK_SIZE};

		bool read_from_socket(Buffer::word_t* res, int32_t msglen) const;

		template <typename T>
		bool read_integral_from_socket(T& x) const
		{
			return read_from_socket(reinterpret_cast<Buffer::word_t*>(&x), sizeof(T));
		}

		bool read_data_from_socket(Buffer::word_t* data, size_t len) const
		{
			return read_from_socket(reinterpret_cast<Buffer::word_t*>(data), static_cast<int32_t>(len));
		}

		void set_socket_provider(std::shared_ptr<CActiveSocket> new_socket);

		CSimpleSocket* get_socket_provider() const;

	public:
		static constexpr int32_t MaximumHeartbeatDelay = 3;
		std::chrono::milliseconds heartBeatInterval = std::chrono::milliseconds(500);

		// region ctor/dtor

		Base(std::string id, Lifetime lifetime, IScheduler* scheduler);

		virtual ~Base() override;

		// endregion

		std::pair<int, sequence_number_t> read_header() const;

		int32_t read_package() const;

		bool read_and_dispatch_message() const;

		void receiverProc() const;

		bool send0(Buffer::ByteArray const& msg, sequence_number_t seqn) const;

		void send(RdId const& rd_id, std::function<void(Buffer& buffer)> writer) const override;

		static bool connection_established(int32_t timestamp, int32_t acknowledged_timestamp);

		std::future<void> start_heartbeat(Lifetime lifetime);

		void ping() const;

		bool send_ack(sequence_number_t seqn) const;

		bool try_shutdown_connection() const;
		
	private:		
		LifetimeDefinition lifetimeDef;
	};

	class RD_FRAMEWORK_API Client : public Base
	{
	public:
		uint16_t port = 0;

		// region ctor/dtor

		Client(Lifetime parentLifetime, IScheduler* scheduler, uint16_t port = 0, const std::string& id = "ClientSocket");

		virtual ~Client() override;
		// endregion

		std::condition_variable_any cv;
	private:		
		LifetimeDefinition clientLifetimeDefinition;
	};

	class RD_FRAMEWORK_API Server : public Base
	{
	public:
		uint16_t port = 0;

		std::unique_ptr<CPassiveSocket> ss;

		// region ctor/dtor

		Server(Lifetime lifetime, IScheduler* scheduler, uint16_t port = 0, const std::string& id = "ServerSocket");

		virtual ~Server() override;
		// endregion
	private:
		LifetimeDefinition serverLifetimeDefinition;
	};
};
}	 // namespace rd
#if defined(_MSC_VER)
#pragma warning(pop)
#endif


#endif	  // RD_CPP_SOCKETWIRE_H
