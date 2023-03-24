#pragma once

#include "lifetime/LifetimeDefinition.h"

#include "Logging/LogMacros.h"
#include "Logging/LogVerbosity.h"
#include "Modules/ModuleInterface.h"
#include "Templates/UniquePtr.h"

DECLARE_LOG_CATEGORY_EXTERN(FLogRiderGameControlModule, Log, All);

class FRiderGameControl;
class FRiderGameControlActionsCache;

class FRiderGameControlModule : public IModuleInterface
{
public:
    FRiderGameControlModule() = default;
    virtual ~FRiderGameControlModule() override = default;

    /** IModuleInterface implementation */
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
    virtual bool SupportsDynamicReloading() override { return true; }

private:
    rd::LifetimeDefinition ModuleLifetimeDefinition;
    TUniquePtr<FRiderGameControl> GameControl;
    TUniquePtr<FRiderGameControlActionsCache> ActionsCache;
};
