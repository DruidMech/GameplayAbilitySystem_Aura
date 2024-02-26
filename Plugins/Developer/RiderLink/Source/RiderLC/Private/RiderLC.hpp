#pragma once

#include "CoreMinimal.h"
#include "Containers/Ticker.h"
#include "lifetime/LifetimeDefinition.h"
#include "Modules/ModuleManager.h"
#include "Runtime/Launch/Resources/Version.h"

DECLARE_LOG_CATEGORY_EXTERN(FLogRiderLCModule, Log, All);

class FRiderLCModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
	virtual bool SupportsDynamicReloading() override { return true; }
	void SetupLiveCodingBinds();
	
private:
	bool Tick(float DeltaTime);
	
	rd::LifetimeDefinition ModuleLifetimeDef;
	FTickerDelegate TickDelegate;
#if ENGINE_MAJOR_VERSION < 5
	FDelegateHandle TickDelegateHandle;
#else
	FTSTicker::FDelegateHandle TickDelegateHandle;
#endif
};
