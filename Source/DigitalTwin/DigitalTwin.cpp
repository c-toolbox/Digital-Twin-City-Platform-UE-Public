// Fill out your copyright notice in the Description page of Project Settings.

#include "DigitalTwin.h"
#include "Modules/ModuleManager.h"
#include "Developer/Settings/Public/ISettingsContainer.h"
#include "Developer/Settings/Public/ISettingsSection.h" 
#include "Developer/Settings/Public/ISettingsModule.h"

#define LOCTEXT_NAMESPACE "FDigitalTwinModule"

IMPLEMENT_PRIMARY_GAME_MODULE(FDigitalTwinModule, DigitalTwin, "DigitalTwin");


void FDigitalTwinModule::StartupModule()
{
}

void FDigitalTwinModule::ShutdownModule()
{
  if (UObjectInitialized()) {
  }
}


#undef LOCTEXT_NAMESPACE