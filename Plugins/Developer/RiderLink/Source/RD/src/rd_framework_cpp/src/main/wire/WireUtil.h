#ifndef RD_CPP_WIREUTIL_H
#define RD_CPP_WIREUTIL_H

#include <cstdint>

#include <rd_framework_export.h>

namespace rd
{
namespace util
{
uint16_t RD_FRAMEWORK_API find_free_port();

void RD_FRAMEWORK_API sleep_this_thread(int64_t ms);
}	 // namespace util
}	 // namespace rd

#endif	  // RD_CPP_WIREUTIL_H
