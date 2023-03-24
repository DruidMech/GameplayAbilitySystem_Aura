#include "ExtWire.h"

#include "protocol/Buffer.h"

namespace rd
{
ExtWire::ExtWire()
{
	connected.advise(Lifetime::Eternal(), [this](bool b) {
		if (b)
		{
			{
				std::lock_guard<decltype(lock)> guard(lock);
				while (true)
				{
					if (sendQ.empty())
					{
						return;
					}
					// auto[id, payload] = std::move(sendQ.front());
					auto it = std::move(sendQ.front());
					sendQ.pop();
					realWire->send(
						it.first, [payload = std::move(it.second)](Buffer& buffer) { buffer.write_byte_array_raw(payload); });
				}
			}
		}
	});
}

void ExtWire::advise(Lifetime lifetime, IRdReactive const* entity) const
{
	realWire->advise(lifetime, entity);
}

void ExtWire::send(RdId const& id, std::function<void(Buffer& buffer)> writer) const
{
	{
		std::lock_guard<decltype(lock)> guard(lock);
		if (!sendQ.empty() || !connected.get())
		{
			Buffer buffer;
			writer(buffer);
			sendQ.emplace(id, buffer.getRealArray());
			return;
		}
	}
	realWire->send(id, std::move(writer));
}
}	 // namespace rd
