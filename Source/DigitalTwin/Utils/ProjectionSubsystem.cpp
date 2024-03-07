// Fill out your copyright notice in the Description page of Project Settings.


#include "DigitalTwin/Utils/ProjectionSubsystem.h"
#include "DigitalTwin/Utils/ProjectionSettings.h"

void UProjectionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
  Super::Initialize(Collection);
  UProjectionSettings* ProjSettings = GetMutableDefault<UProjectionSettings>();
  CameraWorldLocation = FVector(ProjSettings->CameraXOffset,
    ProjSettings->CameraYOffset,
    ProjSettings->CameraHeightAboveGround);
  SetProjectionScreen(ProjSettings->ProjectionScreen);
  SetCameraWorldLocation(CameraWorldLocation);
}

void UProjectionSubsystem::Deinitialize()
{
  Super::Deinitialize();
}

void UProjectionSubsystem::SetCameraWorldLocation(FVector NewCameraWorldLocation) {
  CameraWorldLocation = NewCameraWorldLocation;
  APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
  if (PlayerController) {
    APawn* PlayerPawn = PlayerController->GetPawn();
    if (PlayerPawn) {
      PlayerPawn->SetActorLocation(CameraWorldLocation);
    }
  }

  UProjectionSettings* ProjSettings = GetMutableDefault<UProjectionSettings>();
  if (ProjSettings)
  {
    ProjSettings->CameraXOffset = NewCameraWorldLocation.X;
    ProjSettings->CameraYOffset = NewCameraWorldLocation.Y;
    ProjSettings->CameraHeightAboveGround = NewCameraWorldLocation.Z;
    ProjSettings->SaveConfig();
  }
}

FVector UProjectionSubsystem::GetCameraWorldLocation() const
{
  return CameraWorldLocation;
}

FProjectionScreen UProjectionSubsystem::GetProjectionScreen() const
{
  return ProjectionScreen;
}
