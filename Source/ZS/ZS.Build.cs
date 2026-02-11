// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ZS : ModuleRules
{
	public ZS(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"StateTreeModule",
		
			//카메라 쉐이크
			"GameplayCameras",
			"EngineCameras",
			// Niagara 관련 추가
			"Niagara","NiagaraCore", "NiagaraShader", "RenderCore",
			// AI
			"AIModule",
			"NavigationSystem",
			"StateTreeModule",
			"GameplayStateTreeModule",
			// GAS
			"GameplayAbilities",       
			"GameplayTags",            
			"GameplayTasks",   
			// UI
			"UMG", 
			"Slate",
			"SlateCore",
			"CommonUI", 
			"CommonInput",
			"Paper2D",
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"ZS",
			"ZS/Variant_Platforming",
			"ZS/Variant_Platforming/Animation",
			"ZS/Variant_Combat",
			"ZS/Variant_Combat/AI",
			"ZS/Variant_Combat/Animation",
			"ZS/Variant_Combat/Gameplay",
			"ZS/Variant_Combat/Interfaces",
			"ZS/Variant_Combat/UI",
			"ZS/Variant_SideScrolling",
			"ZS/Variant_SideScrolling/AI",
			"ZS/Variant_SideScrolling/Gameplay",
			"ZS/Variant_SideScrolling/Interfaces",
			"ZS/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
