// Copyright 1998-2020 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class RiderBlueprint : ModuleRules
{
	public RiderBlueprint(ReadOnlyTargetRules Target) : base(Target)
	{
#if UE_4_22_OR_LATER
		PCHUsage = PCHUsageMode.NoPCHs;
#else
		PCHUsage = PCHUsageMode.NoSharedPCHs;
#endif
		
		bUseRTTI = true;
		
		PublicDependencyModuleNames.Add("RD");

		PrivateDependencyModuleNames.AddRange(new []
		{
			"Core",
			"SlateCore",
			"RiderLink",
			"Slate",
			"AssetRegistry",
			"MessagingCommon",
			"UnrealEd",
			"UnrealEdMessages",
			"Engine",
			"CoreUObject"
		});
	}
}
