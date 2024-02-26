#ifndef RD_CPP_PROTOCOL_H
#define RD_CPP_PROTOCOL_H

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable:4251)
#endif

#include "base/IProtocol.h"
#include "protocol/Identities.h"
#include "serialization/SerializationCtx.h"

#include <memory>

#include <rd_framework_export.h>

namespace rd
{
// region predeclared

class SerializationCtx;

class InternRoot;
// endregion

/**
 * \brief Top level node in the object graph. It stores [SerializationCtx] for polymorphic "SerDes"
 */
class RD_FRAMEWORK_API Protocol : /*IRdDynamic, */ public IProtocol
{
	constexpr static string_view InternRootName{"ProtocolInternRoot"};

	Lifetime lifetime;

	mutable std::unique_ptr<SerializationCtx> context;

	mutable std::unique_ptr<InternRoot> internRoot;

	// region ctor/dtor
private:
	void initialize() const;

public:
	Protocol(std::shared_ptr<Identities> identity, IScheduler* scheduler, std::shared_ptr<IWire> wire, Lifetime lifetime);

	Protocol(Identities::IdKind, IScheduler* scheduler, std::shared_ptr<IWire> wire, Lifetime lifetime);

	Protocol(Protocol const&) = delete;

	Protocol(Protocol&&) noexcept = default;

	Protocol& operator=(Protocol&&) noexcept = default;

	virtual ~Protocol();
	// endregion

	SerializationCtx& get_serialization_context() const override;

	static std::shared_ptr<spdlog::logger> initializationLogger;
};
}	 // namespace rd
#if defined(_MSC_VER)
#pragma warning(pop)
#endif


#endif	  // RD_CPP_PROTOCOL_H
