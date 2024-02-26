using UnrealBuildTool;

public class RiderGameControl : ModuleRules
{
	public RiderGameControl(ReadOnlyTargetRules Target) : base(Target)
	{
#if UE_4_22_OR_LATER
		PCHUsage = PCHUsageMode.NoPCHs;
#else
		PCHUsage = PCHUsageMode.NoSharedPCHs;
#endif
		
		bUseRTTI = true;

#if UE_5_2_OR_LATER
		bDisableStaticAnalysis = true;
#endif

		PublicDependencyModuleNames.Add("Core");

		PrivateDependencyModuleNames.AddRange(new []
		{
			"RD",
			"RiderLink",
			"HeadMountedDisplay",
			"LevelEditor",
			"UnrealEd",
			"Slate",
			"CoreUObject",
			"Engine"
		});
	}
}
