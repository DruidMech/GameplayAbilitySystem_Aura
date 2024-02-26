using UnrealBuildTool;

public class RiderShaderInfo : ModuleRules
{
	public RiderShaderInfo(ReadOnlyTargetRules Target) : base(Target)
	{
#if UE_4_22_OR_LATER
		PCHUsage = PCHUsageMode.NoPCHs;
#else
		PCHUsage = PCHUsageMode.NoSharedPCHs;
#endif

#if UE_5_2_OR_LATER
		bDisableStaticAnalysis = true;
#endif

		PrivateDependencyModuleNames.AddRange(new string[] { "Core",  "Projects", "RenderCore" });
	}
}
