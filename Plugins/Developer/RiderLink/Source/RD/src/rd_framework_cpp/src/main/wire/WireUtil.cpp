#include "wire/WireUtil.h"

#include "util/core_util.h"

#include "PassiveSocket.h"
#include "Host.h"

#include <utility>
#include <thread>

namespace rd
{
namespace util
{
uint16_t find_free_port()
{
	CPassiveSocket fake_server;
	fake_server.Initialize();
	fake_server.Listen("127.0.0.1", 0);
	uint16_t port = fake_server.GetServerPort();
	RD_ASSERT_MSG(port != 0, "no free port");
	return port;
}

void sleep_this_thread(int64_t ms)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}
}	 // namespace util
}	 // namespace rd
