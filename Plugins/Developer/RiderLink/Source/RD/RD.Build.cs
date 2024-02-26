using System.IO;
using UnrealBuildTool;

public class RD : ModuleRules
{
	public RD(ReadOnlyTargetRules Target) : base(Target)
	{
		PublicDependencyModuleNames.Add("Core");
		bUseRTTI = true;

#if UE_5_2_OR_LATER
		bDisableStaticAnalysis = true;
#endif

#if UE_5_2_OR_LATER
		IWYUSupport = IWYUSupport.KeepAsIs;
#else
		bEnforceIWYU = false;
#endif

#if UE_4_22_OR_LATER
		CppStandard = CppStandardVersion.Cpp17;
#endif

#if UE_4_22_OR_LATER
		PCHUsage = PCHUsageMode.NoPCHs;
#else
		PCHUsage = PCHUsageMode.NoSharedPCHs;
#endif

#if UE_4_24_OR_LATER
		ShadowVariableWarningLevel = WarningLevel.Off;
		bUseUnity = false;
#else
		bEnableShadowVariableWarnings = false;
		bFasterWithoutUnity = true;
#endif

		PublicDefinitions.Add("_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS");

		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			PublicDefinitions.Add("_WINSOCK_DEPRECATED_NO_WARNINGS");
			PublicDefinitions.Add("_CRT_SECURE_NO_WARNINGS");
			PublicDefinitions.Add("_CRT_NONSTDC_NO_DEPRECATE");
			PublicDefinitions.Add("SPDLOG_WCHAR_FILENAMES");
			PublicDefinitions.Add("SPDLOG_WCHAR_TO_UTF8_SUPPORT");
			PrivateDefinitions.Add("WIN32_LEAN_AND_MEAN");
		}

		if (Target.Platform == UnrealTargetPlatform.Mac)
		{
			PublicDefinitions.Add("_DARWIN");
		}

		// Common dependencies
		PrivateDefinitions.Add("rd_framework_cpp_EXPORTS");
		PrivateDefinitions.Add("rd_core_cpp_EXPORTS");
		PrivateDefinitions.Add("spdlog_EXPORTS");
		PrivateDefinitions.Add("FMT_EXPORT");

		PublicDefinitions.Add("SPDLOG_NO_EXCEPTIONS");
		PublicDefinitions.Add("SPDLOG_COMPILED_LIB");
		PublicDefinitions.Add("SPDLOG_SHARED_LIB");
		PublicDefinitions.Add(
			"nssv_CONFIG_SELECT_STRING_VIEW=nssv_STRING_VIEW_NONSTD");
		PublicDefinitions.Add("FMT_SHARED");

		string[] Paths =
		{
			"src", "src/rd_core_cpp", "src/rd_core_cpp/src/main"
			, "src/rd_framework_cpp", "src/rd_framework_cpp/src/main"
			, "src/rd_framework_cpp/src/main/util", "src/rd_gen_cpp/src"
			, "thirdparty", "thirdparty/ordered-map/include"
			, "thirdparty/optional/tl", "thirdparty/variant/include"
			, "thirdparty/string-view-lite/include", "thirdparty/spdlog/include"
			, "thirdparty/clsocket/src", "thirdparty/CTPL/include"
		};

		foreach (var Item in Paths)
		{
			PublicIncludePaths.Add(Path.Combine(ModuleDirectory, Item));
		}
	}
}
