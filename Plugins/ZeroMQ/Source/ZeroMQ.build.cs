// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System;
using System.IO;
using System.Diagnostics;

public class ZeroMQ : ModuleRules
{
    private string ThirdPartyPath
    {
        
        get { return Path.GetFullPath(Path.Combine(ModuleDirectory, "..", "ThirdParty")); }
        
    }

    private string ZeroMQRootPath
    {
        get { return Path.GetFullPath(Path.Combine(ThirdPartyPath, "libzmq-mt-4_3_5")); }
    }

    public void AddZeroMQ(ReadOnlyTargetRules Target)
    {
        
        
        // add headers
        Console.WriteLine("ZeroMQRootPath : {0} ",ZeroMQRootPath);
        PublicIncludePaths.Add(Path.Combine(ZeroMQRootPath, "include"));
        // tell library that it is statically linked
        foreach (var path in PublicIncludePaths) {
            Console.WriteLine("ZeroMQ Include paths : {0} ", path);
        }
       
        //PublicDefinitions.Add("ZMQ_STATIC");
        string staticLibrary = "";
        if(Target.Platform == UnrealTargetPlatform.Win64) {
            Console.WriteLine("Setting up paths !");
            staticLibrary = Path.Combine(ZeroMQRootPath, "Windows", "x64", "libzmq-mt-4_3_5.lib");
            Console.WriteLine("Using ZeroMQ static library: {0}", staticLibrary);
            PublicAdditionalLibraries.Add(staticLibrary);
            PublicDelayLoadDLLs.Add("libzmq-mt-4_3_5.dll");
            Console.Write(Path.Combine(ZeroMQRootPath, "Windows", "x64", "libzmq-mt-4_3_5.dll"));
            
            RuntimeDependencies.Add(Path.Combine(ZeroMQRootPath, "Windows", "x64", "libzmq-mt-4_3_5.dll"));
            
        } else {
            Console.WriteLine("unsupported target platform: %s", Target.Platform);
            Debug.Assert(false);
        }
        bEnableExceptions = true;
       
    }
    
    public ZeroMQ(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicDependencyModuleNames.AddRange(new string[] { "Core","Projects"});
        AddZeroMQ(Target);
    }
}