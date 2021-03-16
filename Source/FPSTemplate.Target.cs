// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class FPSTemplateTarget : TargetRules
{
	public FPSTemplateTarget(TargetInfo Target) : base(Target)
    {
		Type = TargetType.Game;
        bUsesSteam = true;
        ExtraModuleNames.AddRange(new string[] { "FPSTemplate" });
    }

	//
	// TargetRules interface.
	//

	//public override void SetupBinaries(
	//	TargetInfo Target,
	//	ref List<UEBuildBinaryConfiguration> OutBuildBinaryConfigurations,
	//	ref List<string> OutExtraModuleNames
	//	)
	//{
	//	OutExtraModuleNames.AddRange( new string[] { "FPSTemplate" } );
	//}
}
