// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectionSubsystem.h"
#include "UObject/NoExportTypes.h"
#include "ProjectionSettings.generated.h"

/**
 *
 */
UCLASS(Config = Game)
class DIGITALTWIN_API UProjectionSettings : public UObject
{
  GENERATED_BODY()

public:
  //Values are from test configuration in gothenburg
  UPROPERTY(EditAnywhere, Config, Category = "Projection")
    float CameraHeightAboveGround = 5500.f;
  UPROPERTY(EditAnywhere, Config, Category = "Projection")
    float CameraXOffset = -33.f;
  UPROPERTY(EditAnywhere, Config, Category = "Projection")
    float CameraYOffset = 919.f;
  UPROPERTY(EditAnywhere, Config, Category = "Projection")
    FProjectionScreen ProjectionScreen;

  /*
  UPROPERTY(EditAnywhere, Config, Category = "Projection")
    float CameraHeightAboveGround = 5500.f;
  UPROPERTY(EditAnywhere, Config, Category = "Projection")
    float CameraXOffset = -33.f;
  UPROPERTY(EditAnywhere, Config, Category = "Projection")
    float CameraYOffset = 919.f;
  UPROPERTY(EditAnywhere, Config, Category = "Projection")
    FProjectionScreen ProjectionScreen;
   */

  
#if WITH_EDITOR
  virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
