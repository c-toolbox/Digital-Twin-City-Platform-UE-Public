// Copyright 

#pragma once

#include "CoreMinimal.h"
#include "DigitalTwin/GeoUtils/GeoActor.h"
#include "SwaggerVehicle.h"
#include "GeographicCoordinates.h"

#include "VehicleActor.generated.h"

UENUM(Meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EVehicleActorType : uint8
{
  BOAT = 1 << 0,
  BUS = 1 << 1,
  TAXI = 1 << 2,
  TRAM = 1 << 3,
  TRAIN = 1 << 4,
  UNKNOWN = 1 << 5,
};
ENUM_CLASS_FLAGS(EVehicleActorType);

/**
 *
 */
UCLASS()
class VASTTRAFIK_API AVehicleActor : public AActor
{
  using VehicleType = Swagger::VehicleType;
  GENERATED_BODY()
public:
  AVehicleActor();

protected:
  // Called when the game starts or when spawned
  virtual void BeginPlay() override;

public:
  // Called every frame
  virtual void Tick(float DeltaTime) override;

  virtual void OnConstruction(const FTransform& Transform) override;

  void SetNextCoordinates(double Lon, double Lat);

  UFUNCTION(BlueprintImplementableEvent)
    void SetColor(FLinearColor TransportColor);

  UFUNCTION(BlueprintImplementableEvent)
    void SetBackgroundColor(FLinearColor BgColor);

  void SetJourneyName(const FString& InJourneyName);

  UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Update Journey Name"))
    void ReceiveJourneyName(const FString& InJourneyName);

  void SetInstigator(class ATrafficActor* InTrafficInstigator);

  EVehicleActorType Type;
  FString Gid;
  FString JourneyName;
  float RemoveAfterTime = 5.f;

  UPROPERTY(EditAnywhere)
    bool bRotates = true;
private:
  class ATrafficActor* TrafficInstigator;
  FVector LastLocation;
  FVector NextLocation;
  FGeographicCoordinates LastCoordinates;
  FGeographicCoordinates NextCoordinates;
  FQuat LastAngle;
  FQuat NextAngle;
  float CurrentAlpha = 1.f;
  float TimeBetweenData = 1.f;
  float TimeSinceLastUpdate = 0.f;
  int NumJitters = 0;
};
