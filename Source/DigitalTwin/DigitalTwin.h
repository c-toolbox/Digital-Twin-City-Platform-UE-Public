// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"



/*----------------------------------------------------
              Game module definition
----------------------------------------------------*/
class FDigitalTwinModule : public FDefaultGameModuleImpl
{

public:

  virtual void StartupModule() override;

  virtual void ShutdownModule() override;
};
