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
#include "Engine/DataTable.h"
#include "Logging/LogCategory.h"
#include "AppDesignSettings.h"

#include "Subsystems/EngineSubsystem.h"
#include "AppDesignSubsystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogAppDesign, Log, All);

USTRUCT(BlueprintType, Category = "AppDesign|Fonts")
struct FFontEntry : public FTableRowBase
{
    GENERATED_BODY()
    
public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FSlateFontInfo Font;
};

/**
 * App Design Subsystem
 * Provides support for dynamic design updates to UMG Widgets and other design elements.
 */
UCLASS(DisplayName = "App Design", Category = AppDesign)
class UAppDesignSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()

public:
    UAppDesignSubsystem();
    virtual ~UAppDesignSubsystem();

    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    /// Find Font given a Design Key
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "AppDesign|Fonts")
    bool FindFont(FName Key, FSlateFontInfo & Font) const;

    /// Return the Default Font
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "AppDesign|Fonts")
    void GetDefaultFont(FSlateFontInfo & Font) const { Font = DefaultFont; }

    UPROPERTY()
    const UAppDesignSettings * Settings = 0;

    // Event when Fonts Data Table has updated
    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FFontsChangedDelegate);

    // Event when Fonts Data has changed
    //FFontsChangedEvent & OnFontsChanged() { return FontsChangedEvent; }

    // Dispatcher for Fonts Data has changed
    UPROPERTY(BlueprintAssignable, Category = "AppDesign|Fonts")
    FFontsChangedDelegate OnFontsChanged;

private:

    // Delegate and handle for Fonts Data Table changes
    void OnFontsTableChanged();
    FDelegateHandle FontsTableDelegateHandle;

protected:
    /// Verify Table has correct RowStruct
    bool InitializeFonts();

    UPROPERTY()
    UDataTable * AppFontsTable = 0;
    UPROPERTY()
    FSlateFontInfo DefaultFont;
};
