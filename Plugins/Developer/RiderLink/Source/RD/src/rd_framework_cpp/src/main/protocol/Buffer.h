#ifndef RD_CPP_UNSAFEBUFFER_H
#define RD_CPP_UNSAFEBUFFER_H

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable:4251)
#endif

#include "types/DateTime.h"
#include "util/core_util.h"
#include "types/wrapper.h"
#include "std/allocator.h"
#include "std/list.h"

#include <vector>
#include <type_traits>
#include <functional>
#include <memory>

#include <rd_framework_export.h>

namespace rd
{
/**
 * \brief Simple data buffer. Allows to "SerDes" plenty of types, such as integrals, arrays, etc.
 */
class RD_FRAMEWORK_API Buffer final
{
public:
	friend class PkgInputStream;

	using word_t = uint8_t;

	using Allocator = std::allocator<word_t>;

	using ByteArray = std::vector<word_t, Allocator>;

private:
	template <int>
	friend std::wstring read_wstring_spec(Buffer&);

	template <int>
	friend void write_wstring_spec(Buffer&, wstring_view);

	ByteArray data_;

	size_t offset = 0;

	// read
	void read(word_t* dst, size_t size);

	// write
	void write(const word_t* src, size_t size);

	size_t size() const;

public:
	// region ctor/dtor

	Buffer();

	explicit Buffer(size_t initial_size);

	explicit Buffer(ByteArray array, size_t offset = 0);

	Buffer(Buffer const&) = delete;

	Buffer& operator=(Buffer const&) = delete;

	Buffer(Buffer&&) noexcept = default;

	Buffer& operator=(Buffer&&) noexcept = default;

	// endregion

	size_t get_position() const;

	void set_position(size_t value);

	void require_available(size_t size);

	void check_available(size_t moreSize) const;

	void rewind();

	template <typename T, typename = typename std::enable_if_t<std::is_integral<T>::value, T>>
	T read_integral()
	{
		T result;
		read(reinterpret_cast<word_t*>(&result), sizeof(T));
		return result;
	}

	template <typename T, typename = typename std::enable_if_t<std::is_integral<T>::value>>
	void write_integral(T const& value)
	{
		write(reinterpret_cast<word_t const*>(&value), sizeof(T));
	}

	template <typename T, typename = typename std::enable_if_t<std::is_floating_point<T>::value, T>>
	T read_floating_point()
	{
		T result;
		read(reinterpret_cast<word_t*>(&result), sizeof(T));
		return result;
	}

	template <typename T, typename = typename std::enable_if_t<std::is_floating_point<T>::value>>
	void write_floating_point(T const& value)
	{
		write(reinterpret_cast<word_t const*>(&value), sizeof(T));
	}

	template <template <class, class> class C, typename T, typename A = allocator<T>,
		typename = typename std::enable_if_t<util::is_pod_v<T>>>
	C<T, A> read_array()
	{
		int32_t len = read_integral<int32_t>();
		RD_ASSERT_MSG(len >= 0, "read null array(length = " + std::to_string(len) + ")");
		C<T, A> result;
		using rd::resize;
		resize(result, len);
		if (len > 0)
		{
			read(reinterpret_cast<word_t*>(&result[0]), sizeof(T) * len);
		}
		return result;
	}

	template <template <class, class> class C, typename T, typename A = allocator<value_or_wrapper<T>>>
	C<value_or_wrapper<T>, A> read_array(std::function<value_or_wrapper<T>()> reader)
	{
		int32_t len = read_integral<int32_t>();
		C<value_or_wrapper<T>, A> result;
		using rd::resize;
		resize(result, len);
		for (int32_t i = 0; i < len; ++i)
		{
			result[i] = std::move(reader());
		}
		return result;
	}

	template <template <class, class> class C, typename T, typename A = allocator<T>,
		typename = typename std::enable_if_t<util::is_pod_v<T>>>
	void write_array(C<T, A> const& container)
	{
		using rd::size;
		const int32_t& len = rd::size(container);
		write_integral<int32_t>(static_cast<int32_t>(len));
		if (len > 0)
		{
			write(reinterpret_cast<word_t const*>(&container[0]), sizeof(T) * len);
		}
	}

	template <template <class, class> class C, typename T, typename A = allocator<T>,
		typename = typename std::enable_if_t<!rd::util::in_heap_v<T>>>
	void write_array(C<T, A> const& container, std::function<void(T const&)> writer)
	{
		using rd::size;
		write_integral<int32_t>(size(container));
		for (auto const& e : container)
		{
			writer(e);
		}
	}

	template <template <class, class> class C, typename T, typename A = allocator<Wrapper<T>>>
	void write_array(C<Wrapper<T>, A> const& container, std::function<void(T const&)> writer)
	{
		using rd::size;
		write_integral<int32_t>(size(container));
		for (auto const& e : container)
		{
			writer(*e);
		}
	}

	void read_byte_array(ByteArray& array);

	void read_byte_array_raw(ByteArray& array);

	void write_byte_array_raw(ByteArray const& array);

	//    std::string readString() const;

	//    void writeString(std::string const &value) const;

	bool read_bool();

	void write_bool(bool value);

	wchar_t read_char();

	void write_char(wchar_t value);

	void write_char16_string(const uint16_t* data, size_t len);

	uint16_t * read_char16_string();

	std::wstring read_wstring();

	void write_wstring(std::wstring const& value);

	void write_wstring(wstring_view value);

	void write_wstring(Wrapper<std::wstring> const& value);

	DateTime read_date_time();

	void write_date_time(DateTime const& date_time);

	template <typename T, typename = typename std::enable_if_t<util::is_enum_v<T>>>
	T read_enum()
	{
		int32_t x = read_integral<int32_t>();
		return static_cast<T>(x);
	}

	template <typename T, typename = typename std::enable_if_t<util::is_enum_v<T>>>
	void write_enum(T const& x)
	{
		write_integral<int32_t>(static_cast<int32_t>(x));
	}

	template <typename T, typename = typename std::enable_if_t<util::is_enum_v<T>>>
	T read_enum_set()
	{
		int32_t x = read_integral<int32_t>();
		return static_cast<T>(x);
	}

	template <typename T, typename = typename std::enable_if_t<util::is_enum_v<T>>>
	void write_enum_set(T const& x)
	{
		write_integral<int32_t>(static_cast<int32_t>(x));
	}

	template <typename T, typename F, typename = typename std::enable_if_t<util::is_same_v<typename util::result_of_t<F()>, T>>>
	opt_or_wrapper<T> read_nullable(F&& reader)
	{
		bool nullable = !read_bool();
		if (nullable)
		{
			return {};
		}
		return {reader()};
	}

	template <typename T, typename F,
		typename = typename std::enable_if_t<util::is_same_v<typename util::result_of_t<F()>, Wrapper<T>>>>
	Wrapper<T> read_nullable(F&& reader)
	{
		bool nullable = !read_bool();
		if (nullable)
		{
			return {};
		}
		return reader();
	}

	template <typename T>
	typename std::enable_if_t<!std::is_abstract<T>::value> write_nullable(
		optional<T> const& value, std::function<void(T const&)> writer)
	{
		if (!value)
		{
			write_bool(false);
		}
		else
		{
			write_bool(true);
			writer(*value);
		}
	}

	template <typename T, typename F>
	typename std::enable_if_t<!util::is_invocable_v<F, Wrapper<T>>> write_nullable(Wrapper<T> const& value, F&& writer)
	{
		if (!value)
		{
			write_bool(false);
		}
		else
		{
			write_bool(true);
			writer(*value);
		}
	}

	template <typename T, typename F>
	typename std::enable_if_t<util::is_invocable_v<F, Wrapper<T>>> write_nullable(Wrapper<T> const& value, F&& writer)
	{
		if (!value)
		{
			write_bool(false);
		}
		else
		{
			write_bool(true);
			writer(value);
		}
	}

	ByteArray getArray() const&;

	ByteArray getArray() &&;

	ByteArray getRealArray() const&;

	ByteArray getRealArray() &&;

	word_t const* data() const;

	word_t* data();

	word_t const* current_pointer() const;

	word_t* current_pointer();

	ByteArray& get_data();
};
}	 // namespace rd
#if defined(_MSC_VER)
#pragma warning(pop)
#endif


#endif	  // RD_CPP_UNSAFEBUFFER_H
