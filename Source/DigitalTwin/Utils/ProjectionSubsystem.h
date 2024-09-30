// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "ProjectionSubsystem.generated.h"


USTRUCT(BlueprintType)
struct DIGITALTWIN_API FProjectionScreen
{
  GENERATED_BODY()
    UPROPERTY(Editanywhere)
    FVector LowerLeftCorner = FVector::Zero();
  UPROPERTY(Editanywhere)
    FVector LowerRightCorner = FVector::Zero();
  UPROPERTY(Editanywhere)
    FVector UpperLeftCorner = FVector::Zero();
};


/**
 *
 */
UCLASS()
class DIGITALTWIN_API UProjectionSubsystem : public UWorldSubsystem
{
  GENERATED_BODY()
public:
  void Initialize(FSubsystemCollectionBase& Collection) override;
  void Deinitialize() override;

  void RegisterPlayerActor();

  UFUNCTION(BlueprintCallable, Category = "DigitalTwin|Projection")
    void SetCameraWorldLocation(FVector NewCameraWorldLocation);

  /**
  * Get the world location of the Camera, based on the Project Settings.
  */
  UFUNCTION(BlueprintCallable, Category = "DigitalTwin|Projection")
    FVector GetCameraWorldLocation() const;


  UFUNCTION(BlueprintCallable, Category = "DigitalTwin|Projection")
    void SetProjectionScreen(const FProjectionScreen InProjectionScreen)
  {
    ProjectionScreen = InProjectionScreen;
  }


  UFUNCTION(BlueprintCallable, Category = "DigitalTwin|Projection")
    FProjectionScreen GetProjectionScreen() const;
private:
  //The "ProjectionScreen", or projection surface which represent a 2d plane we will project onto
  UPROPERTY()
    FProjectionScreen ProjectionScreen;

  FVector CameraWorldLocation;
};
