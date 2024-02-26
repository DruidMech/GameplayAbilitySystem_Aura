#ifndef RD_CPP_IRDDYNAMIC_H
#define RD_CPP_IRDDYNAMIC_H

#include "impl/RName.h"

#include <rd_framework_export.h>

namespace rd
{
// region predeclared

class IProtocol;

class SerializationCtx;
// endregion

/**
 * \brief A node in a graph of entities that can be synchronized with its remote copy over a network or
 * a similar connection.
 */
class RD_FRAMEWORK_API IRdDynamic
{
public:
	// region ctor/dtor

	IRdDynamic() = default;

	IRdDynamic(IRdDynamic&& other) = default;

	IRdDynamic& operator=(IRdDynamic&& other) = default;

	virtual ~IRdDynamic() = default;
	// endregion

	virtual const IProtocol* get_protocol() const = 0;

	virtual SerializationCtx& get_serialization_context() const = 0;

	virtual const RName& get_location() const = 0;
};
}	 // namespace rd

#endif	  // RD_CPP_IRDDYNAMIC_H
