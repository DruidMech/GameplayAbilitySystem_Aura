#ifndef RD_CPP_ISERIALIZERSOWNER_H
#define RD_CPP_ISERIALIZERSOWNER_H

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable:4251)
#endif

#include <unordered_set>

#include <rd_framework_export.h>

namespace rd
{
// region predeclared

class Serializers;
// endregion

class RD_FRAMEWORK_API ISerializersOwner
{
	mutable std::unordered_set<Serializers const*> used;

public:
	// region ctor/dtor

	virtual ~ISerializersOwner() = default;
	// endregion

	void registry(Serializers const& serializers) const;

	virtual void registerSerializersCore(Serializers const& serializers) const = 0;
};
}	 // namespace rd
#if defined(_MSC_VER)
#pragma warning(pop)
#endif


#endif	  // RD_CPP_ISERIALIZERSOWNER_H
