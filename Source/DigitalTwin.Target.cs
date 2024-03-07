// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class DigitalTwinTarget : TargetRules
{
    public DigitalTwinTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.V2;

        ExtraModuleNames.AddRange(new string[] { "DigitalTwin", "Vasttrafik" ,"AppDesign" });
    }
}
