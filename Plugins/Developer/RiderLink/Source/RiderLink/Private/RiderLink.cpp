#include "RiderLink.hpp"

#include "ProtocolFactory.h"
#include "UE4Library/UE4Library.Generated.h"

#include "Misc/App.h"
#include "Misc/ScopeRWLock.h"
#include "Modules/ModuleManager.h"
#include "HAL/Platform.h"

#define LOCTEXT_NAMESPACE "RiderLink"

DEFINE_LOG_CATEGORY(FLogRiderLinkModule);

IMPLEMENT_MODULE(FRiderLinkModule, RiderLink);

static FString GetProjectName()
{
	FString ProjectNameNoExtension = FApp::GetProjectName();
	if (ProjectNameNoExtension.IsEmpty())
		ProjectNameNoExtension = TEXT("<ENGINE>");
	return ProjectNameNoExtension;
}

void FRiderLinkModule::ShutdownModule()
{
	UE_LOG(FLogRiderLinkModule, Verbose, TEXT("RiderLink SHUTDOWN START"));
	ModuleLifetimeDef.terminate();
	ProtocolFactory.Reset();
	UE_LOG(FLogRiderLinkModule, Verbose, TEXT("RiderLink SHUTDOWN FINISH"));
}

void FRiderLinkModule::StartupModule()
{
	UE_LOG(FLogRiderLinkModule, Verbose, TEXT("RiderLink STARTUP START"));
	ProtocolFactory = MakeUnique<class ProtocolFactory>(GetProjectName());
	Scheduler.queue([this]()
	{
		InitProtocol();
	});
	UE_LOG(FLogRiderLinkModule, Verbose, TEXT("RiderLink STARTUP FINISH"));
}

void FRiderLinkModule::InitProtocol()
{
	WireLifetimeDef = MakeUnique<rd::LifetimeDefinition>(ModuleLifetimeDef.lifetime);
	rd::Lifetime WireLifetime = WireLifetimeDef->lifetime;
	std::shared_ptr<rd::SocketWire::Server> Wire = ProtocolFactory->CreateWire(&Scheduler, WireLifetime);
	Protocol = ProtocolFactory->CreateProtocol(&Scheduler, WireLifetime.create_nested(), Wire);
	// Exception fired for Server::Base::~Base() when trying to invoke it this way
//	WireLifetime->add_action([this]()
//	{
//		if (!ModuleLifetimeDef.is_terminated())
//		{
//			Scheduler.queue([this]()
//			{
//				InitProtocol();
//			});
//		}
//	});
	Protocol->wire->connected.view(WireLifetime, [this](rd::Lifetime ConnectionLifetime, bool const& IsConnected)
	{
		Scheduler.queue([this, ConnectionLifetime, IsConnected]()
		{
			if (!IsConnected) return;

			FRWScopeLock LockOnConnect(ModelLock, SLT_Write);
			EditorModel = MakeUnique<JetBrains::EditorPlugin::RdEditorModel>();
			EditorModel->connect(ConnectionLifetime, Protocol.Get());
			JetBrains::EditorPlugin::UE4Library::serializersOwner.registerSerializersCore(
				EditorModel->get_serialization_context().get_serializers()
			);
			ConnectionLifetime->add_action([&]() mutable
			{
				Scheduler.queue([&]()mutable
				{
                    FRWScopeLock LockOnDisconnect(ModelLock, SLT_Write);
                    RdIsModelAlive.set(false);
				});
			});
			RdIsModelAlive.set(true);
			
			FString projectName = GetProjectName();
			FString executableName = FPlatformProcess::ExecutableName(false);
			uint32_t pid = FPlatformProcess::GetCurrentProcessId();
			
			std::wstring projectNameWstr = TCHAR_TO_WCHAR(GetData(projectName));
			std::wstring executableNameWstr = TCHAR_TO_WCHAR(GetData(executableName));
			auto connectionInfo = JetBrains::EditorPlugin::ConnectionInfo(projectNameWstr, executableNameWstr, pid);
			EditorModel->get_connectionInfo().set(connectionInfo);
		});
	});
}

bool FRiderLinkModule::SupportsDynamicReloading() { return true; }


// Can't place RdEditorModel or TUniquePtr<RdEditorModel> into RdProperty.
// Have to resort to RdProperty<bool> and change it before creating new RdEditorModel
void FRiderLinkModule::ViewModel(rd::Lifetime Lifetime,
                                 TFunction<void(rd::Lifetime, JetBrains::EditorPlugin::RdEditorModel const&)> Handler)
{
	Scheduler.invoke_or_queue([this, Lifetime, Handler]
	{
		RdIsModelAlive.view(Lifetime, [this, Handler](rd::Lifetime ModelLifetime, bool const& Cond)
		{
			if (Cond) Handler(ModelLifetime, *EditorModel.Get());
		});
	});
}

void FRiderLinkModule::QueueAction(TFunction<void()> Handler)
{
	Scheduler.invoke_or_queue([this, Handler]
	{
		Handler();
	});
}

bool FRiderLinkModule::FireAsyncAction(TFunction<void(JetBrains::EditorPlugin::RdEditorModel const&)> Handler)
{
	FRWScopeLock Lock(ModelLock, SLT_ReadOnly);
	if(!RdIsModelAlive.has_value()) return false;
	
	if(RdIsModelAlive.get())
	{
		Handler(*EditorModel.Get());
	}
	return RdIsModelAlive.get();
}

#undef LOCTEXT_NAMESPACE
