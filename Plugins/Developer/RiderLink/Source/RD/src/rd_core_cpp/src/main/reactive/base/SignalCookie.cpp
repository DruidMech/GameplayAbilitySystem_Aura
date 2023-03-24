#include "SignalCookie.h"

#include <atomic>

namespace
{
std::atomic<int32_t> cookie;
}

void rd_signal_cookie_inc()
{
	++cookie;
}

void rd_signal_cookie_dec()
{
	--cookie;
}

int32_t rd_signal_cookie_get()
{
	return cookie;
}
