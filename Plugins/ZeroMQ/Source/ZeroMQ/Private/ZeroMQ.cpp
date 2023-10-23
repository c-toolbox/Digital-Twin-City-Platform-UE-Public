// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "ZeroMQ/Public/ZeroMQ.h"
#include "Core.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"
#include <zmq.hpp>


#define LOCTEXT_NAMESPACE "FZeroMQModule"

void FZeroMQModule::StartupModule() {
	FString BaseDir = IPluginManager::Get().FindPlugin("ZeroMQ")->GetBaseDir();
	FString ZeroMQ_Dll_dir = FPaths::Combine(*BaseDir, TEXT("ThirdParty/libzmq-mt-4_3_5/Windows/x64/libzmq-mt-4_3_5.dll"));
	DLLHandle = !ZeroMQ_Dll_dir.IsEmpty() ? FPlatformProcess::GetDllHandle(*ZeroMQ_Dll_dir) : nullptr;
	if (DLLHandle) {
		int major, minor, patch = 0;
		zmq::version(&major,&minor,&patch);
		UE_LOG(LogTemp, Warning, TEXT("ZeroMQ version: v%d.%d.%d"), major, minor, patch);
	} else {
		FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("ThirdPartyLibraryError", "Failed to load MyAwesomeLibrary"));
	}
}

void FZeroMQModule::ShutdownModule() {
	FPlatformProcess::FreeDllHandle(DLLHandle);
}

#undef LOCTEXT_NAMESPACE
IMPLEMENT_MODULE(FZeroMQModule, ZeroMQ)
