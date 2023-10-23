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

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "AppDesignSettings.generated.h"

/**
 * AppDesignSettings
 */
UCLASS(Config = Game, DefaultConfig)
class UAppDesignSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
    /// Table of Fonts, available through FindFont
    UPROPERTY(Config, EditAnywhere, Category = AppDesign, Meta=(AllowedClasses = "DataTable"))
    FSoftObjectPath AppFontsTable;
    
    /// Default Font to use when Font Key is missing
    UPROPERTY(Config, EditAnywhere, Category = AppDesign)
    FSlateFontInfo DefaultFont;
};
