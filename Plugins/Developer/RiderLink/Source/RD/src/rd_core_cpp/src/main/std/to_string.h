#ifndef RD_CPP_TO_STRING_H
#define RD_CPP_TO_STRING_H

#include <string>
#include <type_traits>
#include <thread>
#include <sstream>
#include <vector>
#include <atomic>
#include <future>
#include <locale>
#if defined(_MSC_VER) || defined(__APPLE__)
#include <codecvt>
#else
#include <limits>
#include <iconv.h>
#endif

#include <thirdparty.hpp>

namespace rd
{
namespace detail
{
using std::to_string;

inline std::string to_string(std::string const& val)
{
	return val;
}

inline std::string to_string(const char* val)
{
	return val;
}

#if defined(_MSC_VER) || defined(__APPLE__)
template<class I, class E, class S>
struct codecvt : std::codecvt<I, E, S>
{
	~codecvt()
	{ }
};

inline std::string to_string(std::wstring const& val)
{
#if defined(__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif
	using convert_type = codecvt<wchar_t, char, std::mbstate_t>;
	std::wstring_convert<convert_type> converter;
	return converter.to_bytes(val);
#if defined(__APPLE__)
#pragma clang diagnostic pop
#endif
}
#else
const std::string conv_error("Conversion Error");
inline std::string to_string(const std::wstring& wstr) {
	std::string result;
	if (wstr.empty()) {
		return result;
	}
	// Order of arguments is to, from
	auto icvt = iconv_open("UTF-8", "WCHAR_T");
	// CentOS is not happy with -1
	if (std::numeric_limits<iconv_t>::max() == icvt) {
		return conv_error;
	}

	// I hope this does not modify the incoming buffer
	wchar_t* non_const_in = const_cast<wchar_t*>(wstr.c_str());
	char* iconv_in = reinterpret_cast<char*>(non_const_in);
	size_t iconv_in_bytes = wstr.length() * sizeof(wchar_t);
	// Temp buffer, assume every code point converts into 3 bytes, this should be enough
	// We do not convert terminating zeros
	const size_t buffer_len = wstr.length() * 3;
	auto buffer = std::make_unique<char[]>(buffer_len);

	char* iconv_out = buffer.get();
	size_t iconv_out_bytes = buffer_len;
	auto ret = iconv(icvt, &iconv_in, &iconv_in_bytes, &iconv_out, &iconv_out_bytes);
	if (static_cast<size_t>(-1) == ret) {
		result = conv_error;
	} else {
		size_t converted_len = buffer_len - iconv_out_bytes;
		result.assign(buffer.get(), converted_len);
	}
	iconv_close(icvt);
	return result;
}
#endif

inline std::string to_string(std::thread::id const& id)
{
	std::ostringstream ss;
	ss << id;
	return ss.str();
}

inline std::string to_string(std::exception const& e)
{
	return std::string(e.what());
}

inline std::string to_string(std::future_status const& status)
{
	switch (status)
	{
		case std::future_status::ready:
			return "ready";
		case std::future_status::timeout:
			return "timeout";
		case std::future_status::deferred:
			return "deferred";
		default:
			return "unknown";
	}
}

template <typename Rep, typename Period>
inline std::string to_string(std::chrono::duration<Rep, Period> const& time)
{
	return std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(time).count()) + "ms";
}

template <typename T>
inline std::string to_string(T const* val)
{
	return val ? to_string(*val) : "nullptr";
}

template <typename T>
inline std::string to_string(std::atomic<T> const& value)
{
	return to_string(value.load());
}

template <typename T>
inline std::string to_string(optional<T> const& val)
{
	if (val.has_value())
	{
		return to_string(*val);
	}
	else
	{
		return "nullopt";
	}
}

template <typename F, typename S>
inline std::string to_string(const std::pair<F, S> p)
{
	return "(" + to_string(p.first) + ", " + to_string(p.second) + ")";
}

template <template <class, class> class C, typename T, typename A>
std::string to_string(C<T, A> const& v)
{
	std::string res = "[";
	for (const auto& item : v)
	{
		res += to_string(item);
		res += ",";
	}
	res += "]";
	return res;
}

template <class T>
std::string as_string(T const& t)
{
	return to_string(t);
}

using std::to_wstring;

inline std::wstring to_wstring(std::string const& s)
{
	// TO-DO: fix this wrong implementation
	return std::wstring(s.begin(), s.end());
}

template <class T>
std::wstring as_wstring(T const& t)
{
	return to_wstring(t);
}
}	 // namespace detail

template <typename T>
std::string to_string(T const& val)
{
	return detail::as_string(val);
}

template <typename T>
std::wstring to_wstring(T const& val)
{
	return detail::as_wstring(val);
}
}	 // namespace rd

#endif	  // RD_CPP_TO_STRING_H
