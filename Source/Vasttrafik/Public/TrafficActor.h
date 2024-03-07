// Copyright 

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/IHttpRequest.h" 
#include "SwaggerLivemapApi.h"
#include "SwaggerLivemapApiOperations.h"
#include "VehicleActor.h"

#include "TrafficActor.generated.h"

//This is an oauth2-response
//USTRUCT for easier json deserialization
USTRUCT()
struct FAuthResponse
{
  GENERATED_BODY()
public:
  UPROPERTY()
    FString scope;
  UPROPERTY()
    FString token_type;
  UPROPERTY()
    double expires_in;
  UPROPERTY()
    FString access_token;
};

//TODO: This might be a "dynamic dataset" in the end, and should register to the datasetsubsystem
UCLASS()
class VASTTRAFIK_API ATrafficActor : public AInfo
{
  GENERATED_BODY()

public:
  // Sets default values for this actor's properties
  ATrafficActor();

protected:
  // Called when the game starts or when spawned
  virtual void BeginPlay() override;

public:
  // Called every frame
  virtual void Tick(float DeltaTime) override;

  void OnAuthFinished(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);
  void OnLiveMapResponse(Swagger::SwaggerLivemapApi::LivemapResponse& Response);

  //This is called by each vehicle, since they know when to die
  void RequestRemoval(class AVehicleActor* Vehicle);

  void ToggleTraffic(bool bActiveState);

  UFUNCTION(BlueprintCallable)
    void ActivateVehicleType(EVehicleActorType VehicleActorType);

  UFUNCTION(BlueprintCallable)
    void DeactivateVehicleType(EVehicleActorType VehicleActorType);

  UFUNCTION(BlueprintCallable)
    void ToggleVehicleType(EVehicleActorType VehicleActorType);

  UPROPERTY(EditAnywhere)
    bool bIsActive = true;

  UPROPERTY(EditAnywhere)
    float SecondsBetweenRequests = 1.0;

  UPROPERTY(EditAnywhere)
    TSubclassOf<class AVehicleActor> BusActorType;

  UPROPERTY(EditAnywhere)
    TSubclassOf<class AVehicleActor> TramActorType;

  //There are "regional" and "long distance" trains. Should we make a difference?
  UPROPERTY(EditAnywhere)
    TSubclassOf<class AVehicleActor> TrainActorType;

  UPROPERTY(EditAnywhere)
    TSubclassOf<class AVehicleActor> BoatActorType;

  UPROPERTY(EditAnywhere)
    TSubclassOf<class AVehicleActor> TaxiActorType;

  UPROPERTY(VisibleAnywhere)
    TMap<FString, class AVehicleActor*> VehicleActorMap;

  UPROPERTY(EditAnywhere, Meta = (Bitmask, BitmaskEnum = "EVehicleActorType"))
    uint8 ActiveVehicleTypes = 0x0; //Disable all vehicle types by default
private:

  //Sends an OAUTH2-authentication request to the västtrafik API.
  //I (mikpe) totally winged this implementation, hope it works!
  bool Authenticate();

  //Request the livemap data. This requires a valid token, so we must call "Authenticate" first.
  void RequestLivemap();

  bool bIsAuthenticating = false;

  UPROPERTY()
    FAuthResponse Authentication;

  Swagger::SwaggerLivemapApi::FLivemapDelegate Delegate;

  TArray<Swagger::SwaggerVehicle> Vehicles;
  float SecondsSinceLastRequest = 1000.0;
  FDateTime LastLivemapUpdate;
};
