#include "RiderBlueprint.hpp"

#include "BlueprintProvider.hpp"
#include "IRiderLink.hpp"
#include "Model/RdEditorProtocol/RdEditorModel/RdEditorModel.Generated.h"


#include "Engine/Blueprint.h"
#include "Framework/Docking/TabManager.h"
#include "HAL/PlatformProcess.h"
#include "MessageEndpoint.h"
#include "MessageEndpointBuilder.h"
#include "Modules/ModuleManager.h"
#include "Runtime/Launch/Resources/Version.h"

#if ENGINE_MAJOR_VERSION < 5
#include "AssetRegistryModule.h"
#else
#include "AssetRegistry/AssetRegistryModule.h"
#endif

#define LOCTEXT_NAMESPACE "RiderLink"

DEFINE_LOG_CATEGORY(FLogRiderBlueprintModule);

IMPLEMENT_MODULE(FRiderBlueprintModule, RiderBlueprint);

static void AllowSetForeGroundForEditor(JetBrains::EditorPlugin::RdEditorModel const & unrealToBackendModel) {
    static const int32 CurrentProcessId = FPlatformProcess::GetCurrentProcessId();
    try {
        const rd::WiredRdTask<bool> Task = unrealToBackendModel.get_allowSetForegroundWindow().sync(CurrentProcessId);
        if (Task.is_faulted()) {
            UE_LOG(FLogRiderBlueprintModule, Error, TEXT("AllowSetForeGroundForEditor failed: %hs "), rd::to_string(Task.value_or_throw()).c_str());
        }
        else if (Task.is_succeeded()) {
            if (!(Task.value_or_throw().unwrap())) {
                UE_LOG(FLogRiderBlueprintModule, Error, TEXT("AllowSetForeGroundForEditor failed: %hs "), rd::to_string(Task.value_or_throw()).c_str());
            }
        }
    }
    catch (std::exception const &e) {
        UE_LOG(FLogRiderBlueprintModule, Error, TEXT("AllowSetForeGroundForEditor failed: %hs "), rd::to_string(e).c_str());
    }
}

void FRiderBlueprintModule::StartupModule()
{
    UE_LOG(FLogRiderBlueprintModule, Verbose, TEXT("STARTUP START"));
    IRiderLinkModule& RiderLinkModule = IRiderLinkModule::Get();
    ModuleLifetimeDef = RiderLinkModule.CreateNestedLifetimeDefinition();

    const FAssetRegistryModule* AssetRegistryModule = &FModuleManager::LoadModuleChecked<FAssetRegistryModule>
        (AssetRegistryConstants::ModuleName);

    MessageEndpoint = FMessageEndpoint::Builder(FName("FAssetEditorManager")).Build();

    AssetRegistryModule->Get().OnAssetAdded().AddLambda([](const FAssetData& AssetData) {
        // TO-DO: Fix loading uasset's on 4.23-
        // BluePrintProvider::AddAsset(AssetData);
    });

    RiderLinkModule.ViewModel(ModuleLifetimeDef.lifetime, [this] (rd::Lifetime ModelLifetime, JetBrains::EditorPlugin::RdEditorModel const& UnrealToBackendModel)
    {
        UnrealToBackendModel.get_openBlueprint().advise(
            ModelLifetime,
            [this, &UnrealToBackendModel](
            JetBrains::EditorPlugin::BlueprintReference const& s)
            {
                try
                {
                    AllowSetForeGroundForEditor(UnrealToBackendModel);

                    auto Window = FGlobalTabmanager::Get()->GetRootWindow();
                    if (!Window.IsValid()) return;

                    if (Window->IsWindowMinimized())
                    {
                        Window->Restore();
                    }
                    else
                    {
                        Window->HACK_ForceToFront();
                    }
                    BluePrintProvider::OpenBlueprint(s, MessageEndpoint);
                }
                catch (std::exception const& e)
                {
                    std::cerr << rd::to_string(e);
                }
            }
        );

        UnrealToBackendModel.get_isBlueprintPathName().set([](FString const& pathName) -> bool
        {
            return BluePrintProvider::IsBlueprint(pathName);
        });
    });
    UE_LOG(FLogRiderBlueprintModule, Verbose, TEXT("STARTUP FINISH"));
}

void FRiderBlueprintModule::ShutdownModule()
{
    UE_LOG(FLogRiderBlueprintModule, Verbose, TEXT("SHUTDOWN START"));
    ModuleLifetimeDef.terminate();
    UE_LOG(FLogRiderBlueprintModule, Verbose, TEXT("SHUTDOWN FINISH"));
}
