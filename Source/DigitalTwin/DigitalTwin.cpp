// Fill out your copyright notice in the Description page of Project Settings.

#include "DigitalTwin.h"
#include "Modules/ModuleManager.h"
#include "Utils/ProjectionSettings.h"
#include "Developer/Settings/Public/ISettingsContainer.h"
#include "Developer/Settings/Public/ISettingsSection.h" 
#include "Developer/Settings/Public/ISettingsModule.h"

#define LOCTEXT_NAMESPACE "FDigitalTwinModule"

IMPLEMENT_PRIMARY_GAME_MODULE(FDigitalTwinModule, DigitalTwin, "DigitalTwin");


void FDigitalTwinModule::StartupModule()
{
  RegisterSettings();
}

void FDigitalTwinModule::ShutdownModule()
{
  if (UObjectInitialized()) {
    UnregisterSettings();
  }
}


bool FDigitalTwinModule::HandleSettingsSaved()
{
#if WITH_EDITORONLY_DATA
  UProjectionSettings* ProjectionSettings = GetMutableDefault<UProjectionSettings>();
  ProjectionSettings->SaveConfig();
  return true;
#endif
  return false;
}

void FDigitalTwinModule::RegisterSettings()
{
#if WITH_EDITORONLY_DATA
  if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
  {
    ISettingsSectionPtr SettingsSection = SettingsModule->RegisterSettings("Project", "DigitalTwin", "Projection",
      LOCTEXT("RuntimeGeneralName", "Projection Settings"),
      LOCTEXT("RuntimeGeneralDescription", "Projection settings for the Digital Twin Project"),
      GetMutableDefault<UProjectionSettings>());

    if (SettingsSection.IsValid()) {
      SettingsSection->OnModified().BindRaw(this, &FDigitalTwinModule::HandleSettingsSaved);
    }
  }
#endif
}

void FDigitalTwinModule::UnregisterSettings()
{
#if WITH_EDITORONLY_DATA
  if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings")) {
    SettingsModule->UnregisterSettings("Project", "DigitalTwin", "Projection");
  }
#endif
}

#undef LOCTEXT_NAMESPACE