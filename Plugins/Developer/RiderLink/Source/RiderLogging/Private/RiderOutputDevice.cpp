#include "RiderOutputDevice.hpp"

#include "CoreGlobals.h"
#include "Misc/ScopeLock.h"
#include "Misc/OutputDeviceRedirector.h"

void FRiderOutputDevice::Serialize(const TCHAR* V, ELogVerbosity::Type Verbosity, const FName& Category)
{
	Serialize(V, Verbosity, Category, {});
}

void FRiderOutputDevice::Serialize(const TCHAR* V, ELogVerbosity::Type Verbosity, const FName& Category,
                                   const double Time)
{
	FScopeLock Lock{&CriticalSection};
	
	onSerializeMessage.ExecuteIfBound(V, Verbosity, Category, {Time});
}

void FRiderOutputDevice::Setup(TFunction<FOnSerializeMessage::TFuncType> Callback)
{
	FScopeLock Lock{&CriticalSection};

	if(onSerializeMessage.IsBound()) return;
	
	onSerializeMessage.BindLambda(Callback);
	GLog->AddOutputDevice(this);
	GLog->SerializeBacklog(this);
}

void FRiderOutputDevice::TearDown()
{
	{
		FScopeLock Lock{&CriticalSection};
	
		if(onSerializeMessage.IsBound() == false) return;
	
		onSerializeMessage.Unbind();
	}
	
	// At shutdown, GLog may already be null
	// This should be out of scoped lock, because it'll implicitly try take a lock in OutputDeviceRedirector
	// And Serialize is already holding that lock, so we'll have a deadlock here
	if (GLog != nullptr)
	{
		GLog->RemoveOutputDevice(this);
	}
}
