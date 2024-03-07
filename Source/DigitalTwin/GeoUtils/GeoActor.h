// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DecalActor.h"
#include "GeographicCoordinates.h"
#include "GeoActor.generated.h"

//The GeoActor is intended to be a georeferenced actor, translating a lon/lat to
//a position in the engine scene and vice versa. 
//This actor is using the GeoReferencingSubsystem for these transformations.
UCLASS()
class DIGITALTWIN_API AGeoActor : public AActor
{
  GENERATED_BODY()

public:
  // Sets default values for this actor's properties
  AGeoActor();

protected:
  // Called when the game starts or when spawned
  virtual void BeginPlay() override;

public:
  // Called every frame
  virtual void Tick(float DeltaTime) override;

  virtual void OnConstruction(const FTransform& Transform) override;

#if WITH_EDITOR
  virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

  UPROPERTY(EditAnywhere)
    double Wgs84Longitude;
  UPROPERTY(EditAnywhere)
    double Wgs84Latitude;

  //Since this type is not really UPROPERTY-friendly, we translate in tick
  FGeographicCoordinates GeoCoordinates;
private:

  void UpdateActorLocation();
};
