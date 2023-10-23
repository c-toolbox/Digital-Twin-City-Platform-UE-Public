// Fill out your copyright notice in the Description page of Project Settings.


#include "GeoActor.h"
#include "GeoReferencingSystem.h"
#include "GeographicCoordinates.h"
#include "CartesianCoordinates.h"

// Sets default values
AGeoActor::AGeoActor()
  :Super()
{
  // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
  PrimaryActorTick.bCanEverTick = true;
}


// Called when the game starts or when spawned
void AGeoActor::BeginPlay()
{
  Super::BeginPlay();
}

// Called every frame
void AGeoActor::Tick(float DeltaTime)
{
  Super::Tick(DeltaTime);
  if (GeoCoordinates.Longitude != Wgs84Longitude && GeoCoordinates.Latitude != Wgs84Latitude)
  {
    GeoCoordinates.Longitude = Wgs84Longitude;
    GeoCoordinates.Latitude = Wgs84Latitude;
    UpdateActorLocation();
  }
}

void AGeoActor::OnConstruction(const FTransform& Transform)
{
  Super::OnConstruction(Transform);

  //TBD: If we always use the internal centimeter-scale this transformation is not needed
  const FVector Location = Transform.GetLocation() * 100.0;
  auto GeoRefSystem = AGeoReferencingSystem::GetGeoReferencingSystem(GetWorld());
  if (GeoRefSystem)
  {
    FCartesianCoordinates Coords;
    GeoRefSystem->EngineToProjected(Location, Coords);
    GeoRefSystem->ProjectedToGeographic(Coords, GeoCoordinates);
    Wgs84Latitude = GeoCoordinates.Latitude;
    Wgs84Longitude = GeoCoordinates.Longitude;
  }
}
#if WITH_EDITOR
void AGeoActor::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
  //Get the name of the property that was changed  
  FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;

  if (PropertyName == GET_MEMBER_NAME_CHECKED(AGeoActor, Wgs84Longitude) ||
    PropertyName == GET_MEMBER_NAME_CHECKED(AGeoActor, Wgs84Latitude))
  {
    GeoCoordinates.Longitude = Wgs84Longitude;
    GeoCoordinates.Latitude = Wgs84Latitude;
    UpdateActorLocation();
  }

  // Call the base class version  
  Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

void AGeoActor::UpdateActorLocation() {
  auto GeoRefSystem = AGeoReferencingSystem::GetGeoReferencingSystem(GetWorld());
  if (GeoRefSystem)
  {
    FCartesianCoordinates Coords;
    GeoRefSystem->GeographicToProjected(GeoCoordinates, Coords);
    FVector Location;
    GeoRefSystem->ProjectedToEngine(Coords, Location);
    SetActorLocation(Location / 100.0);
  }
}
