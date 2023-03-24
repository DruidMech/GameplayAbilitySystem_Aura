#ifndef RD_CPP_POLYMORPHIC_H
#define RD_CPP_POLYMORPHIC_H

#include "protocol/Buffer.h"
#include "base/RdReactiveBase.h"

#include <type_traits>

namespace rd
{
// region predeclared

class SerializationCtx;
// endregion

/**
 * \brief Maintains "SerDes" for statically polymorphic type [T].
 * Requires static "read" and "write" methods as in common case below.
 * \tparam T type to "SerDes"
 * \tparam R trait specialisation (void by default)
 */
template <typename T, typename R = void>
class Polymorphic
{
public:
	inline static T read(SerializationCtx& ctx, Buffer& buffer)
	{
		return T::read(ctx, buffer);
	}

	inline static void write(SerializationCtx& ctx, Buffer& buffer, T const& value)
	{
		value.write(ctx, buffer);
	}

	inline static void write(SerializationCtx& ctx, Buffer& buffer, Wrapper<T> const& value)
	{
		value->write(ctx, buffer);
	}
};

template <typename T>
class Polymorphic<T, typename std::enable_if_t<std::is_integral<T>::value>>
{
public:
	inline static T read(SerializationCtx& /*ctx*/, Buffer& buffer)
	{
		return buffer.read_integral<T>();
	}

	inline static void write(SerializationCtx& /*ctx*/, Buffer& buffer, T const& value)
	{
		buffer.write_integral<T>(value);
	}
};

template <typename T>
class Polymorphic<T, typename std::enable_if_t<std::is_floating_point<T>::value>>
{
public:
	inline static T read(SerializationCtx& /*ctx*/, Buffer& buffer)
	{
		return buffer.read_floating_point<T>();
	}

	inline static void write(SerializationCtx& /*ctx*/, Buffer& buffer, T const& value)
	{
		buffer.write_floating_point<T>(value);
	}
};

// class Polymorphic<int, void>;

template <template <class, class> class C, typename T, typename A>
class Polymorphic<C<T, A>, typename std::enable_if_t<!util::is_base_of_v<RdReactiveBase, T> &&
													 !util::is_same_v<Wrapper<T, A>, C<T, A>>>>
{
public:
	inline static C<T, A> read(SerializationCtx& /*ctx*/, Buffer& buffer)
	{
		return buffer.read_array<C, T, A>();
	}

	inline static void write(SerializationCtx& /*ctx*/, Buffer& buffer, C<T, A> const& value)
	{
		buffer.write_array<C, T, A>(value);
	}
};

template <>
class Polymorphic<bool>
{
public:
	inline static bool read(SerializationCtx& /*ctx*/, Buffer& buffer)
	{
		return buffer.read_bool();
	}

	inline static void write(SerializationCtx& /*ctx*/, Buffer& buffer, bool const& value)
	{
		buffer.write_bool(value);
	}
};

template <>
class Polymorphic<wchar_t>
{
public:
	inline static wchar_t read(SerializationCtx& /*ctx*/, Buffer& buffer)
	{
		return buffer.read_char();
	}

	inline static void write(SerializationCtx& /*ctx*/, Buffer& buffer, bool const& value)
	{
		buffer.write_char(value);
	}
};

template <>
class Polymorphic<std::wstring>
{
public:
	inline static std::wstring read(SerializationCtx& /*ctx*/, Buffer& buffer)
	{
		return buffer.read_wstring();
	}

	inline static void write(SerializationCtx& /*ctx*/, Buffer& buffer, std::wstring const& value)
	{
		buffer.write_wstring(value);
	}

	inline static void write(SerializationCtx& /*ctx*/, Buffer& buffer, Wrapper<std::wstring> const& value)
	{
		buffer.write_wstring(*value);
	}
};

template <>
class Polymorphic<DateTime>
{
public:
	inline static DateTime read(SerializationCtx& /*ctx*/, Buffer& buffer)
	{
		return buffer.read_date_time();
	}

	inline static void write(SerializationCtx& /*ctx*/, Buffer& buffer, DateTime const& value)
	{
		buffer.write_date_time(value);
	}
};

template <>
class Polymorphic<Void>
{
public:
	inline static Void read(SerializationCtx& /*ctx*/, Buffer& /*buffer*/)
	{
		return {};
	}

	inline static void write(SerializationCtx& /*ctx*/, Buffer& /*buffer*/, Void const& /*value*/)
	{
	}
};

template <typename T>
class Polymorphic<T, typename std::enable_if_t<util::is_base_of_v<RdReactiveBase, T>>>
{
public:
	inline static T read(SerializationCtx& ctx, Buffer& buffer)
	{
		return T::read(ctx, buffer);
	}

	inline static void write(SerializationCtx& ctx, Buffer& buffer, T const& value)
	{
		value.write(ctx, buffer);
	}
};

template <typename T>
class Polymorphic<T, typename std::enable_if_t<util::is_enum_v<T>>>
{
public:
	inline static T read(SerializationCtx& /*ctx*/, Buffer& buffer)
	{
		return buffer.read_enum<T>();
	}

	inline static void write(SerializationCtx& /*ctx*/, Buffer& buffer, T const& value)
	{
		buffer.write_enum<T>(value);
	}
};

template <typename T>
class Polymorphic<optional<T>>
{
public:
	inline static optional<T> read(SerializationCtx& ctx, Buffer& buffer)
	{
		return buffer.read_nullable<T>([&ctx, &buffer]() { return Polymorphic<T>::read(ctx, buffer); });
	}

	inline static void write(SerializationCtx& ctx, Buffer& buffer, optional<T> const& value)
	{
		buffer.write_nullable<T>(value, [&ctx, &buffer](T const& v) { Polymorphic<T>::write(ctx, buffer, v); });
	}
};

template <typename T, typename A>
class Polymorphic<Wrapper<T, A>>
{
public:
	inline static void write(SerializationCtx& ctx, Buffer& buffer, Wrapper<T, A> const& value)
	{
		value->write(ctx, buffer);
	}
};
}	 // namespace rd

#endif	  // RD_CPP_POLYMORPHIC_H
