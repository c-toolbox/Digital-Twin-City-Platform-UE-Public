// --------------------------------------------------------------------------------
// Copyright 2021, Linköping University and Universeum AB
//
// Authors:
//   Patric Ljung, patric.ljung@liu.se
//
// Description:
//   This is a TextBlock aware of the AppDesign Subsystem, in every other aspect
//   it should be drop-in replacement of the Default Text widget.
//
// Notes:
//   Updates from the AppFontsTable (AppDesign Subsystem) should be
//   captured by the active widget in Designer mode.
// --------------------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "Components/TextBlock.h"
#include "AppDesignTextBlock.generated.h"

class UAppDesignSubsystem;

/**
 * AppDesign Text Block is a TextBlock widget aware of the AppDesign Subsystem
 * that will be used to request Design properties from it, such as a Font Style to use.
 */
UCLASS(Meta = (DisplayName="ADText"))
class UAppDesignTextBlock : public UTextBlock
{
	GENERATED_BODY()
	
public:
    /**
     * Dynamically set the Font Key Style using AppDesign Subsystem
     *
     * @param InStyleKey The Key for the Font style to be picked
     */
    UFUNCTION(BlueprintCallable, Category = "AppDesign")
    void SetFontStyleKey(const FName InStyleKey);

public:
    /* The Font Style Key */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AppDesign")
    FName FontStyleKey { TEXT("Default") };

    /**
     * Synchronize the Font Style Key property by requesting the Keyd Font Style and assign it.
     */
    virtual void SynchronizeProperties() override;

    //~ Begin UObject Interface
    virtual void PostLoad() override;
    virtual void BeginDestroy() override;
    //~ End UObject Interface

#if WITH_EDITOR
    // OnFontsChanged callback is added here
    virtual void ConnectEditorData() override;
#endif

protected:
    // Pointer to AppDesign Subsystem
    UAppDesignSubsystem * AppDesign = 0;

    // Apply the FontStyleRef, retrive font style and assign to font
    void ApplyFontStyleKey();

    // Delegate called when AppDesign
    UFUNCTION()
    void OnFontsChanged();
};
