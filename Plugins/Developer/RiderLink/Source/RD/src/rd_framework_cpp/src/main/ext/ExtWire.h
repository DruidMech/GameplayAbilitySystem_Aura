#ifndef RD_CPP_EXTWIRE_H
#define RD_CPP_EXTWIRE_H

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable:4251)
#endif

#include "base/IWire.h"
#include "protocol/RdId.h"
#include "protocol/Buffer.h"

#include <queue>
#include <mutex>
#include <functional>

#include <rd_framework_export.h>

namespace rd
{
class RD_FRAMEWORK_API ExtWire final : public IWire
{
	mutable std::mutex lock;

	mutable std::queue<std::pair<RdId, Buffer::ByteArray> > sendQ;

public:
	ExtWire();

	mutable IWire const* realWire = nullptr;

	void advise(Lifetime lifetime, IRdReactive const* entity) const override;

	void send(RdId const& id, std::function<void(Buffer& buffer)> writer) const override;
};
}	 // namespace rd
#if defined(_MSC_VER)
#pragma warning(pop)
#endif


#endif	  // RD_CPP_EXTWIRE_H
