#include "ProtocolFactory.h"

#include "scheduler/base/IScheduler.h"
#include "wire/SocketWire.h"

#include "Runtime/Launch/Resources/Version.h"

#if ENGINE_MAJOR_VERSION >= 5
#include "HAL/PlatformFileManager.h"
#else
#include "HAL/PlatformFilemanager.h"
#endif
#include "Misc/App.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

#if PLATFORM_WINDOWS
// ReSharper disable once CppUnusedIncludeDirective
#include "Windows/AllowWindowsPlatformTypes.h"
#include "Windows/PreWindowsApi.h"

#include "Windows/WindowsPlatformMisc.h"

#include "Windows/PostWindowsApi.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "Windows/HideWindowsPlatformTypes.h"
#endif

#include "spdlog/sinks/daily_file_sink.h"

static FString GetLocalAppdataFolder()
{
    const FString EnvironmentVarName =
#if PLATFORM_WINDOWS
TEXT("LOCALAPPDATA");
#else
    TEXT("HOME");
#endif
#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION <= 20
    TCHAR CAppDataLocalPath[4096];
    FPlatformMisc::GetEnvironmentVariable(*EnvironmentVarName, CAppDataLocalPath, ARRAY_COUNT(CAppDataLocalPath));
    return CAppDataLocalPath;
#else
    return FPlatformMisc::GetEnvironmentVariable(*EnvironmentVarName);
#endif
}

static FString GetMiscFilesFolder()
{    
    FString FAppDataLocalPath = GetLocalAppdataFolder();
    FPaths::NormalizeFilename(FAppDataLocalPath);
    return FPaths::Combine(*FAppDataLocalPath,
#if PLATFORM_WINDOWS
        TEXT("Jetbrains"), TEXT("Rider"), TEXT("Unreal")
#elif PLATFORM_MAC
        TEXT("Library"), TEXT("Logs"), TEXT("Unreal Engine")
#else
        TEXT(".config"), TEXT("unrealEngine")
#endif
    );
}

static FString GetPathToPortsFolder()
{
    const FString MiscFilesFolder = GetMiscFilesFolder();
    return FPaths::Combine(*MiscFilesFolder, TEXT("Ports"));
}

static FString GetLogFile(const FString& projectName)
{
    const FString MiscFilesFolder = GetMiscFilesFolder();
    return FPaths::Combine(*MiscFilesFolder, TEXT("Logs"), projectName + TEXT(".uproject"));
}

ProtocolFactory::ProtocolFactory(const FString& ProjectName): ProjectName(ProjectName)
{
    InitRdLogging();
}

void ProtocolFactory::InitRdLogging()
{
    spdlog::set_level(spdlog::level::err);
#if defined(ENABLE_LOG_FILE) && ENABLE_LOG_FILE == 1
    const FString LogFile = GetLogFile(ProjectName);
    const FString Msg = TEXT("[RiderLink] Path to log file: ") + LogFile;
    auto FileLogger = std::make_shared<spdlog::sinks::daily_file_sink_mt>(*LogFile, 23, 59);
    FileLogger->set_level(spdlog::level::trace);
    spdlog::apply_all([FileLogger](std::shared_ptr<spdlog::logger> Logger)
    {
        Logger->sinks().push_back(FileLogger);
    });
#endif
}

std::shared_ptr<rd::SocketWire::Server> ProtocolFactory::CreateWire(rd::IScheduler* Scheduler, rd::Lifetime SocketLifetime)
{
    return std::make_shared<rd::SocketWire::Server>(SocketLifetime, Scheduler, 0,
                                                         TCHAR_TO_UTF8(*FString::Printf(TEXT("UnrealEditorServer-%s"),
                                                             *ProjectName)));
}


TUniquePtr<rd::Protocol> ProtocolFactory::CreateProtocol(rd::IScheduler* Scheduler, rd::Lifetime SocketLifetime, std::shared_ptr<rd::SocketWire::Server> wire)
{
    auto protocol = MakeUnique<rd::Protocol>(rd::Identities::SERVER, Scheduler, wire, SocketLifetime);

    auto& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
    const FString PortFullDirectoryPath = GetPathToPortsFolder();
    if (PlatformFile.CreateDirectoryTree(*PortFullDirectoryPath) && !IsRunningCommandlet())
    {
        const FString ProjectFileName = ProjectName + TEXT(".uproject");
        const FString TmpPortFile = TEXT("~") + ProjectFileName;
        const FString TmpPortFileFullPath = FPaths::Combine(*PortFullDirectoryPath, *TmpPortFile);
        FFileHelper::SaveStringToFile(FString::FromInt(wire->port), *TmpPortFileFullPath);
        const FString PortFileFullPath = FPaths::Combine(*PortFullDirectoryPath, *ProjectFileName);
        IFileManager::Get().Move(*PortFileFullPath, *TmpPortFileFullPath, true, true);
    }
    return protocol;
}
