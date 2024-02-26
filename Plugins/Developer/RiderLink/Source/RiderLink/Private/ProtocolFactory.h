#pragma once

#include <protocol/Protocol.h>
#include "wire/SocketWire.h"

#include "Containers/UnrealString.h"
#include "Templates/UniquePtr.h"

class ProtocolFactory
{
public:
	explicit ProtocolFactory(const FString& ProjectName);

	std::shared_ptr<rd::SocketWire::Server> CreateWire(rd::IScheduler* Scheduler, rd::Lifetime SocketLifetime);
	TUniquePtr<rd::Protocol> CreateProtocol(rd::IScheduler* Scheduler, rd::Lifetime SocketLifetime,
	                                        std::shared_ptr<rd::SocketWire::Server> wire);

private:
	void InitRdLogging();

private:
	FString ProjectName;
};
