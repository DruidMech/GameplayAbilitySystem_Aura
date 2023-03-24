#pragma once

#include "Misc/OutputDevice.h"
#include "Delegates/Delegate.h"
#include "Logging/LogVerbosity.h"

DECLARE_DELEGATE_FourParams(FOnSerializeMessage, const TCHAR*, ELogVerbosity::Type, const class FName&, TOptional<double>);

class FRiderOutputDevice : public FOutputDevice {
public:
	FRiderOutputDevice();
	virtual ~FRiderOutputDevice() override;

	FOnSerializeMessage onSerializeMessage;

protected:
	virtual void Serialize(const TCHAR* V, ELogVerbosity::Type Verbosity, const class FName& Category) override;

	virtual void Serialize(const TCHAR* V, ELogVerbosity::Type Verbosity, const class FName& Category, double Time) override;
};
