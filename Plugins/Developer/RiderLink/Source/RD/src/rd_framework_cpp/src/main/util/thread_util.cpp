#include "thread_util.h"

#ifdef _WIN32

#include <windows.h>
const DWORD MS_VC_EXCEPTION = 0x406D1388;

#pragma pack(push, 8)
typedef struct tagTHREADNAME_INFO
{
	DWORD dwType;		 // Must be 0x1000.
	LPCSTR szName;		 // Pointer to name (in user addr space).
	DWORD dwThreadID;	 // Thread ID (-1=caller thread).
	DWORD dwFlags;		 // Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack(pop)

void SetThreadName(uint32_t dwThreadID, const char* threadName)
{
	THREADNAME_INFO info;
	info.dwType = 0x1000;
	info.szName = threadName;
	info.dwThreadID = dwThreadID;
	info.dwFlags = 0;

	__try
	{
		RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*) &info);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
	}
}

void SetThreadName(const char* threadName)
{
	SetThreadName(GetCurrentThreadId(), threadName);
}

#elif defined(__APPLE__)

#include <pthread.h>

void SetThreadName(const char* threadName)
{
	pthread_setname_np(threadName);
}

#else

#include <pthread.h>

void SetThreadName(const char* threadName)
{
	pthread_setname_np(pthread_self(), threadName);
}

#endif

namespace rd
{
namespace util
{
void set_thread_name(const char* name)
{
	SetThreadName(name);
}

}	 // namespace util
}	 // namespace rd
