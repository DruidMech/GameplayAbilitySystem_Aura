#include "RiderOutputDevice.hpp"

#include "CoreGlobals.h"
#include "Misc/OutputDeviceRedirector.h"

FRiderOutputDevice::FRiderOutputDevice() {
	GLog->AddOutputDevice(this);
	GLog->SerializeBacklog(this);
}

FRiderOutputDevice::~FRiderOutputDevice() {
	if (onSerializeMessage.IsBound())
		onSerializeMessage.Unbind();
	// At shutdown, GLog may already be null
	if (GLog != nullptr) {
		GLog->RemoveOutputDevice(this);
	}
}

void FRiderOutputDevice::Serialize(const TCHAR* V, ELogVerbosity::Type Verbosity, const FName& Category) {
	onSerializeMessage.ExecuteIfBound(V, Verbosity, Category, {});
}

void FRiderOutputDevice::Serialize(const TCHAR* V, ELogVerbosity::Type Verbosity, const FName& Category,
                                   const double Time) {
	onSerializeMessage.ExecuteIfBound(V, Verbosity, Category, {Time});
}