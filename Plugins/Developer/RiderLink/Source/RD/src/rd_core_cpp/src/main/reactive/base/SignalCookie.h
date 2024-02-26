#ifndef RD_CPP_SIGNALCOOKIE_H
#define RD_CPP_SIGNALCOOKIE_H

#include <cstdint>
#include <rd_core_export.h>

extern "C" void RD_CORE_API rd_signal_cookie_inc();
extern "C" void RD_CORE_API rd_signal_cookie_dec();
extern "C" int32_t RD_CORE_API rd_signal_cookie_get();

#endif	  // RD_CPP_SIGNALCOOKIE_H
