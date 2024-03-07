// Fill out your copyright notice in the Description page of Project Settings.


#include "DigitalTwin/Utils/ProjectionSettings.h"
#include "DigitalTwin/Utils/ProjectionSubsystem.h"

#if WITH_EDITOR

void UProjectionSettings::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
  //Get the name of the property that was changed
  GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2, FColor::Orange, TEXT("Changed projection properties!"));
  FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
  UWorld* World = Cast<UWorld>(GetOuter());
  //UWorld* World = GetWorld();
  if (World) {
    GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2, FColor::Orange, TEXT("World found!"));
    UProjectionSubsystem* ProjectionSubsystem = World->GetSubsystem<UProjectionSubsystem>();
    if (ProjectionSubsystem)
    {
      GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2, FColor::Orange, TEXT("ProjectionSubsystem found!"));
      if (PropertyName == GET_MEMBER_NAME_CHECKED(UProjectionSettings, CameraHeightAboveGround) ||
        PropertyName == GET_MEMBER_NAME_CHECKED(UProjectionSettings, CameraXOffset) ||
        PropertyName == GET_MEMBER_NAME_CHECKED(UProjectionSettings, CameraYOffset))
      {
        ProjectionSubsystem->SetCameraWorldLocation({ CameraXOffset, CameraYOffset, CameraHeightAboveGround });
      }
    }
  }

  // Call the base class version  
  Super::PostEditChangeProperty(PropertyChangedEvent);
}

#endif