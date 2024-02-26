#include "RiderLC.hpp"

#include "IRiderLink.hpp"
#include "RdEditorModel/RdEditorModel.Generated.h"

#include "Async/Async.h"
#if WITH_LIVE_CODING
#include "ILiveCodingModule.h"
#endif
#include "Misc/HotReloadInterface.h"

#define LOCTEXT_NAMESPACE "FRiderLCModule"

DEFINE_LOG_CATEGORY(FLogRiderLCModule);

const FName HotReloadModule("HotReload");

void FRiderLCModule::SetupLiveCodingBinds()
{
	IRiderLinkModule& RiderLinkModule = IRiderLinkModule::Get();
	RiderLinkModule.ViewModel(ModuleLifetimeDef.lifetime, [](const rd::Lifetime& Lifetime, JetBrains::EditorPlugin::RdEditorModel const& RdEditorModel)
	{
		RdEditorModel.get_triggerHotReload().advise(Lifetime, []
		{
			AsyncTask(ENamedThreads::GameThread, []
			{
#if WITH_LIVE_CODING
				ILiveCodingModule* LiveCoding = FModuleManager::GetModulePtr<ILiveCodingModule>(LIVE_CODING_MODULE_NAME);
				if (LiveCoding != nullptr && LiveCoding->IsEnabledByDefault())
				{
					LiveCoding->EnableForSession(true);
					if (LiveCoding->IsEnabledForSession())
					{
						LiveCoding->Compile();
					}
				}
				else
#endif
				{
#if WITH_HOT_RELOAD
					IHotReloadInterface* HotReload = FModuleManager::GetModulePtr<IHotReloadInterface>(HotReloadModule);
					if (HotReload != nullptr && !HotReload->IsCurrentlyCompiling())
					{
						HotReload->DoHotReloadFromEditor(EHotReloadFlags::None);
					}
#endif
				}
			});
		});
	});
}

bool FRiderLCModule::Tick(float DeltaTime)
{
	bool bIsAvailable = false;
	bool bIsCompiling = false;
#if WITH_LIVE_CODING
	const ILiveCodingModule* LiveCoding = FModuleManager::GetModulePtr<ILiveCodingModule>(LIVE_CODING_MODULE_NAME);
	if (LiveCoding != nullptr && LiveCoding->IsEnabledByDefault())
	{
		bIsAvailable = true;
		bIsCompiling = LiveCoding->IsCompiling();
	}
	else
#endif
	{
#if WITH_HOT_RELOAD
		const IHotReloadInterface* HotReload = FModuleManager::GetModulePtr<IHotReloadInterface>(HotReloadModule);
		if (HotReload != nullptr)
		{
			bIsAvailable = true;
			bIsCompiling = HotReload->IsCurrentlyCompiling();
		}
#endif
	}
	IRiderLinkModule& RiderLinkModule = IRiderLinkModule::Get();
	RiderLinkModule.QueueModelAction([bIsAvailable, bIsCompiling](JetBrains::EditorPlugin::RdEditorModel const& RdEditorModel)
	{
		RdEditorModel.get_isHotReloadAvailable().set(bIsAvailable);
		RdEditorModel.get_isHotReloadCompiling().set(bIsCompiling);
	});

	return true;
}

void FRiderLCModule::StartupModule()
{
	UE_LOG(FLogRiderLCModule, Verbose, TEXT("RiderLC STARTUP START"));
	
	const IRiderLinkModule& RiderLinkModule = IRiderLinkModule::Get();
	ModuleLifetimeDef = RiderLinkModule.CreateNestedLifetimeDefinition();
	SetupLiveCodingBinds();
	TickDelegate = FTickerDelegate::CreateRaw(this, &FRiderLCModule::Tick);
#if ENGINE_MAJOR_VERSION < 5
	TickDelegateHandle = FTicker::GetCoreTicker().AddTicker(TickDelegate);
#else
	TickDelegateHandle = FTSTicker::GetCoreTicker().AddTicker(TickDelegate);
#endif
	
	UE_LOG(FLogRiderLCModule, Verbose, TEXT("RiderLC STARTUP FINISH"));
}

void FRiderLCModule::ShutdownModule()
{
	UE_LOG(FLogRiderLCModule, Verbose, TEXT("RiderLC SHUTDOWN START"));

#if ENGINE_MAJOR_VERSION < 5
	FTicker::GetCoreTicker().RemoveTicker(TickDelegateHandle);
#else
	FTSTicker::GetCoreTicker().RemoveTicker(TickDelegateHandle);
#endif
	ModuleLifetimeDef.terminate();
	
	UE_LOG(FLogRiderLCModule, Verbose, TEXT("RiderLC SHUTDOWN FINISH"));
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FRiderLCModule, RiderLC)