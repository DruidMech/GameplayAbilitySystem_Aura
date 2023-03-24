#include "RiderShaderInfo.h"

#include "HAL/FileManager.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/FileHelper.h"
#include "Modules/ModuleManager.h"
#include "ShaderCore.h"

IMPLEMENT_MODULE(FRiderShaderInfoModule, RiderShaderInfo);

void FRiderShaderInfoModule::StartupModule()
{
	const TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin(TEXT("RiderLink"));
	if(!Plugin.IsValid()) return;
	
	const FString MappingFile = FPaths::Combine(Plugin->GetBaseDir(), TEXT("Intermediate"), TEXT("FileSystemMappings.ini"));
	const FString TmpMappingFile = FPaths::Combine(Plugin->GetBaseDir(), TEXT("Intermediate"), TEXT("~FileSystemMappings.ini"));

	const TMap<FString, FString> ShaderMappings = AllShaderSourceDirectoryMappings();
	const TArray<FString> Mappings = [&ShaderMappings]()->TArray<FString>
	{
		TArray<FString> Result;
		for(const TTuple<FString, FString>& Pair : ShaderMappings)
		{
			Result.Add(FString::Printf(TEXT("%s=%s"), *Pair.Key,  *FPaths::ConvertRelativePathToFull(Pair.Value)));
		}
		return Result;
	}();
	if(IFileManager::Get().FileExists(*MappingFile))
	{
		TArray<FString> Result;
		FFileHelper::LoadFileToStringArray(Result, *MappingFile);
		if(Mappings == Result) return;
	}
	FFileHelper::SaveStringArrayToFile(Mappings, *TmpMappingFile);
	IFileManager::Get().Move(*MappingFile, *TmpMappingFile, true, true);
}
