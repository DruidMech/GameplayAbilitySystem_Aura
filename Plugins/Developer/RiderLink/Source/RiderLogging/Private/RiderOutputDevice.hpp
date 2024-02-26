#pragma once

#include "Misc/OutputDevice.h"
#include "Delegates/Delegate.h"
#include "Logging/LogVerbosity.h"

DECLARE_DELEGATE_FourParams(FOnSerializeMessage, const TCHAR*, ELogVerbosity::Type, const FName&, TOptional<double>);

class FRiderOutputDevice : public FOutputDevice {
public:
	void Setup(TFunction<FOnSerializeMessage::TFuncType>);
	virtual void TearDown() override;

protected:
	virtual void Serialize(const TCHAR* V, ELogVerbosity::Type Verbosity, const FName& Category) override;
	virtual void Serialize(const TCHAR* V, ELogVerbosity::Type Verbosity, const FName& Category, double Time) override;
	
private:
	FOnSerializeMessage onSerializeMessage;
	FCriticalSection CriticalSection;
};
