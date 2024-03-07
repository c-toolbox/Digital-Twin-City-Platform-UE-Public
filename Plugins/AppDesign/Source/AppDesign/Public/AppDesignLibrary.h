// --------------------------------------------------------------------------------
// Copyright 2021, Linköping University and Universeum AB
//
// Authors:
//   Patric Ljung, patric.ljung@liu.se
//
// Description:
//   This AppDesignSubsystem is giving developers and designers a means
//   dynamically assign design styles to UI elements, such as Fonts.
//
// Package:
//   AppDesign/AppDesign - Module
//   AppDesign/AppDesignSubsystem - Engine Subsystem
//   AppDesign/AppDesignSettings - Configuration
//   AppDesign/AppDesignLibrary - Blueprint Library Interface
//
// Background:
//   This code was originally developed in the Vislabs project for the
//   VEINS application.
// --------------------------------------------------------------------------------

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "AppDesignLibrary.generated.h"

UCLASS()
class UAppDesignLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
#if 0
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Execute Sample function", Keywords = "AppDesign sample test testing"), Category = "AppDesignTesting")
	static float AppDesignSampleFunction(float Param);
#endif
};
