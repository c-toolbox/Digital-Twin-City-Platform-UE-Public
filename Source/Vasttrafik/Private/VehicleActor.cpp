// Copyright 


#include "VehicleActor.h"
#include "TrafficActor.h"
#include "GeoReferencingSystem.h"
#include "CartesianCoordinates.h"

AVehicleActor::AVehicleActor()
  :Super()
{
  PrimaryActorTick.bCanEverTick = true;
}
// Called when the game starts or when spawned
void AVehicleActor::BeginPlay()
{
  Super::BeginPlay();
}

// Called every frame
void AVehicleActor::Tick(float DeltaTime)
{
  TimeSinceLastUpdate += DeltaTime;
  if (TimeSinceLastUpdate > RemoveAfterTime && TrafficInstigator)
  {
    TrafficInstigator->RequestRemoval(this);
  }

  CurrentAlpha = FMath::Clamp(CurrentAlpha + DeltaTime, 0.f, 1.f);

  FVector Location = FMath::Lerp(LastLocation, NextLocation, CurrentAlpha);
  if (bRotates) {
    FQuat Rotation = FQuat::Slerp(LastAngle, NextAngle, CurrentAlpha);
    SetActorLocationAndRotation(Location, Rotation);
  }
  else {
    SetActorLocation(Location);
  }

  //Delay blueprint tick until after the location/rotation has been set.
  Super::Tick(DeltaTime);
}

void AVehicleActor::OnConstruction(const FTransform& Transform)
{
  Super::OnConstruction(Transform);
}

void AVehicleActor::SetNextCoordinates(double Lon, double Lat)
{
  auto World = GetWorld();
  auto GeoRefSystem = AGeoReferencingSystem::GetGeoReferencingSystem(World);
  if (GeoRefSystem)
  {
    const FVector TmpLastLocation = LastLocation;
    LastLocation = NextLocation;
    FCartesianCoordinates Coords;
    //TODO: Find height of terrain:
    FGeographicCoordinates GeoCoords(Lon, Lat, 0.0);
    GeoRefSystem->GeographicToProjected(GeoCoords, Coords);
    GeoRefSystem->ProjectedToEngine(Coords, NextLocation);
    NextLocation /= 100.f;

    FHitResult HitResult;
    if (World->LineTraceSingleByChannel(HitResult, NextLocation + FVector::UpVector * 100.0, NextLocation + FVector::DownVector * 100.0, ECC_GameTraceChannel1)) {
      NextLocation.Z = HitResult.ImpactPoint.Z;
    }
    if (FVector::Distance(LastLocation, NextLocation) > 50.0) {
      NumJitters += 1;
      if (NumJitters > 3) {
        LastLocation = NextLocation;
      }
      else {
        NextLocation = LastLocation;
      }
    }
    else {
      NumJitters = 0;
    }
    LastAngle = NextAngle;
    FVector Res = NextLocation - LastLocation;
    if (Res.Size2D() > SMALL_NUMBER)
    {
      Res.Normalize();
      float Angle = FMath::Atan2(Res.Y, Res.X);
      NextAngle = FQuat(FVector::UpVector, Angle);
    }
  }

  TimeSinceLastUpdate = 0.0;
  CurrentAlpha = 0.0;
}

void AVehicleActor::SetJourneyName(const FString& InJourneyName)
{
  JourneyName = InJourneyName;
  ReceiveJourneyName(InJourneyName);
}

void AVehicleActor::SetInstigator(ATrafficActor* InTrafficInstigator)
{
  TrafficInstigator = InTrafficInstigator;
}
