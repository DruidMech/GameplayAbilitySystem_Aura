#ifndef RD_CPP_FRAMEWORK_IDENTITIES_H
#define RD_CPP_FRAMEWORK_IDENTITIES_H

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable:4251)
#endif

#include "protocol/RdId.h"

#include <atomic>

#include <rd_framework_export.h>

namespace rd
{
/**
 * \brief Generates unique identifiers for objects in an object graph.
 */
class RD_FRAMEWORK_API Identities
{
private:
	mutable std::atomic_int32_t id_acc;

public:
	enum class IdKind
	{
		Client,
		Server
	};

	constexpr static IdKind SERVER = IdKind::Server;
	constexpr static IdKind CLIENT = IdKind::Client;

	constexpr static int32_t BASE_CLIENT_ID = RdId::MAX_STATIC_ID;

	constexpr static int32_t BASE_SERVER_ID = RdId::MAX_STATIC_ID + 1;

	// region ctor/dtor

	explicit Identities(IdKind dynamicKind);

	virtual ~Identities() = default;
	// endregion

	/**
	 * \brief Generates the next unique identifier.
	 * \param parent previous id which is used for generating.
	 * \return unique identifier.
	 */
	RdId next(const RdId& parent) const;
};
}	 // namespace rd
#if defined(_MSC_VER)
#pragma warning(pop)
#endif


#endif	  // RD_CPP_FRAMEWORK_IDENTITIES_H
