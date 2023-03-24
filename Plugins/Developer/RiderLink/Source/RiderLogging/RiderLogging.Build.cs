// Copyright 1998-2020 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class RiderLogging : ModuleRules
{
	public RiderLogging(ReadOnlyTargetRules Target) : base(Target)
	{
#if UE_4_22_OR_LATER
		PCHUsage = PCHUsageMode.NoPCHs;
#else
		PCHUsage = PCHUsageMode.NoSharedPCHs;
#endif
		
		bUseRTTI = true;

		PrivateDependencyModuleNames.AddRange(new []
		{
			"Core",
			"RD",
			"RiderLink",
			"RiderBlueprint"
		});
	}
}
