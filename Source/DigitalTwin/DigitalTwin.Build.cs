// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class DigitalTwin : ModuleRules
{
    public DigitalTwin(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { 
            "Core", 
            "CoreUObject", 
            "Engine",
            "RHI",
            "Slate",
            "InputCore",
            "Json",
            "JsonUtilities",
            "MediaAssets",
            "GeometryCore",
            "GeoReferencing",
            "RemoteControl",
            "ZeroMQ",
            "WebSockets"
          
        });

        PrivateDependencyModuleNames.AddRange(new string[] { });

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        //PrivateDependencyModuleNames.Add( new string(){});

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}
