// --------------------------------------------------------------------------------
// Copyright 2021, Linköping University and Universeum AB
//
// Authors:
//   Patric Ljung, patric.ljung@liu.se
//
// Package: AppDesign
// --------------------------------------------------------------------------------

#include "AppDesignTextBlock.h"
#include "AppDesignSubsystem.h"
#include "HAL/ThreadManager.h"

void UAppDesignTextBlock::SetFontStyleKey(const FName InStyleKey)
{
    FontStyleKey = InStyleKey;
    SynchronizeProperties();
}

void UAppDesignTextBlock::SynchronizeProperties()
{
    if (AppDesign)
    {
        ApplyFontStyleKey();
    }

    //UE_LOG(LogTemp, Log, TEXT("ADText::SynchronizeProperties> this = %p"), this);

    Super::SynchronizeProperties();
}

void UAppDesignTextBlock::BeginDestroy()
{
    if (AppDesign)
    {
        // Need to remove callback here, as widget will be destroyed.
        AppDesign->OnFontsChanged.RemoveDynamic(this, &UAppDesignTextBlock::OnFontsChanged);
        AppDesign = 0;
    }

    //UE_LOG(LogTemp, Log, TEXT("ADText::BeginDestroy> this = %p"), this);

    Super::BeginDestroy();
}

void UAppDesignTextBlock::PostLoad()
{
    Super::PostLoad();
    
    //UE_LOG(LogTemp, Log, TEXT("ADText::PostLoad> this = %p"), this);

    if (AppDesign)
    {
        ApplyFontStyleKey();
    }
}

#if WITH_EDITOR

void UAppDesignTextBlock::ConnectEditorData()
{
    Super::ConnectEditorData();

    AppDesign = GEngine->GetEngineSubsystem<UAppDesignSubsystem>();

    if (AppDesign)
    {
        // Setup the OnFontsChanged callback for live design editing
        AppDesign->OnFontsChanged.AddDynamic(this, &UAppDesignTextBlock::OnFontsChanged);
    }

    //UE_LOG(LogTemp, Log, TEXT("ADText::ConnectEditorData> this = %p"), this);
}

#endif

// --------------------------------------------------------------------------------
// INTERNAL FUNCTIONS
// --------------------------------------------------------------------------------

void UAppDesignTextBlock::ApplyFontStyleKey()
{
    // Use AppDesign Subsystem to get Font if Key is set
    if (!FontStyleKey.IsNone())
    {
        if (!AppDesign->FindFont(FontStyleKey, Font))
        {
            UE_LOG(LogTemp, Warning, TEXT("Font Key not found: %s"),
                   *FontStyleKey.ToString());
            AppDesign->GetDefaultFont(Font);
        }
    }
}

void UAppDesignTextBlock::OnFontsChanged()
{
    ApplyFontStyleKey();
    //UE_LOG(LogTemp, Log, TEXT("ADText::OnFontsChanged> this = %p"), this);
    SynchronizeProperties();
}
