#ifndef RD_CPP_ISERIALIZABLE_H
#define RD_CPP_ISERIALIZABLE_H

#include <string>

#include <rd_framework_export.h>

namespace rd
{
// region predeclared

class Buffer;

class SerializationCtx;
// endregion

/**
 * \brief Provides \ref write for serialization to be overriden. For deserialization derived class must have static
 * method read. See examples for more information.
 */
class RD_FRAMEWORK_API ISerializable
{
public:
	virtual ~ISerializable() = default;

	virtual void write(SerializationCtx& ctx, Buffer& buffer) const = 0;
};

/**
 * \brief Dynamically polymorhic node.
 */
class RD_FRAMEWORK_API IPolymorphicSerializable : public ISerializable
{
public:
	/**
	 * \return actual class's name as written in source code.
	 */
	virtual std::string type_name()
		const = 0 /*{ throw std::invalid_argument("type doesn't support polymorphic serialization"); }*/;

	//		virtual bool equals(IPolymorphicSerializable const& object) const = 0;

	virtual size_t hashCode() const noexcept;

	virtual std::string toString() const = 0;

	virtual bool equals(ISerializable const&) const = 0;

	friend bool RD_FRAMEWORK_API operator==(const IPolymorphicSerializable& lhs, const IPolymorphicSerializable& rhs);

	friend bool RD_FRAMEWORK_API operator!=(const IPolymorphicSerializable& lhs, const IPolymorphicSerializable& rhs);
};
}	 // namespace rd

namespace std
{
template <>
struct RD_FRAMEWORK_API hash<rd::IPolymorphicSerializable>
{
	size_t operator()(const rd::IPolymorphicSerializable& value) const noexcept
	{
		return value.hashCode();
	}
};
}	 // namespace std

#endif	  // RD_CPP_ISERIALIZABLE_H
