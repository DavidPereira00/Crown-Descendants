// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class FPSTemplate : ModuleRules
{
	public FPSTemplate(ReadOnlyTargetRules Target) : base(Target)
    {
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "UMG", "Slate", "SlateCore", "OnlineSubsystem", "OnlineSubsystemNull", "OnlineSubsystemUtils" });
        PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
        DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");

        // Uncomment if you are using online features
        if ((Target.Platform == UnrealTargetPlatform.Win32) || (Target.Platform == UnrealTargetPlatform.Win64))
        {
            if (true/*UEBuildConfiguration.bCompileSteamOSS == true*/)
            {
                DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");
            }
        }
    }
}
