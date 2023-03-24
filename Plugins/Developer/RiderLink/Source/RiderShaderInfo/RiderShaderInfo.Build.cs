// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

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
		PrivateDependencyModuleNames.AddRange(new string[] { "Core",  "Projects", "RenderCore" });
	}
}
