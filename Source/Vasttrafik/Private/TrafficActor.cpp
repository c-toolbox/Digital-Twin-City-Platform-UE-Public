// Copyright 


#include "TrafficActor.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h" 
#include "VasttrafikModule.h"
#include "HttpModule.h"
#include "JsonObjectConverter.h" 
#include "SwaggerLivemap.h"
#include "GeoReferencingSystem.h"
#include "GeographicCoordinates.h"
#include "DigitalTwin/Datasets/DatasetCatalog.h"
#include "DigitalTwin/Datasets/DatasetSubsystem.h"
#include "TrafficDataset.h"

#include "Interfaces/IHttpResponse.h"


const FString APIBasePath = "https://api.vasttrafik.se/bin/rest.exe/v2";
const FString APITokenURL = "https://api.vasttrafik.se:443/token";

#ifdef WITH_EDITOR
const FString DEVICE_ID = "EditorDevice";
#else
const FString DEVICE_ID = "ShippingDevice";
#endif



// Sets default values
ATrafficActor::ATrafficActor()
  : Super()
{
  // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
  PrimaryActorTick.bCanEverTick = true;

  Delegate.BindUObject(this, &ATrafficActor::OnLiveMapResponse);
}

// Called when the game starts or when spawned
void ATrafficActor::BeginPlay()
{
  Super::BeginPlay();
  UDatasetCatalog* TrafficCatalog = NewObject<UDatasetCatalog>();
  TrafficCatalog->SetCatalogName(TEXT("vasttrafik_livemap"));

  UTrafficDataset* BusDataset = NewObject<UTrafficDataset>();
  BusDataset->VehicleType = EVehicleActorType::BUS;
  BusDataset->TrafficActor = this;
  BusDataset->SetDatasetName("bus");
  UTrafficDataset* TramDataset = NewObject<UTrafficDataset>();
  TramDataset->VehicleType = EVehicleActorType::TRAM;
  TramDataset->TrafficActor = this;
  TramDataset->SetDatasetName("tram");
  UTrafficDataset* TrainDataset = NewObject<UTrafficDataset>();
  TrainDataset->VehicleType = EVehicleActorType::TRAIN;
  TrainDataset->TrafficActor = this;
  TrainDataset->SetDatasetName("train");
  UTrafficDataset* BoatDataset = NewObject<UTrafficDataset>();
  BoatDataset->VehicleType = EVehicleActorType::BOAT;
  BoatDataset->TrafficActor = this;
  BoatDataset->SetDatasetName("boat");

  TrafficCatalog->AddDataset(BusDataset);
  TrafficCatalog->AddDataset(TramDataset);
  TrafficCatalog->AddDataset(TrainDataset);
  TrafficCatalog->AddDataset(BoatDataset);
  auto World = GetWorld();
  if (World) {
    UDatasetSubsystem* DatasetSubsystem = World->GetSubsystem<UDatasetSubsystem>();
    DatasetSubsystem->RegisterCatalog(TrafficCatalog);
  }
}

// Called every frame
void ATrafficActor::Tick(float DeltaTime)
{
  Super::Tick(DeltaTime);
  Authentication.expires_in -= DeltaTime;

  if (Authentication.expires_in <= 0.0 && !bIsAuthenticating) {
    const bool AuthRequestResult = Authenticate();
    if (!AuthRequestResult) {
      bIsAuthenticating = false;
    }
  }

  if (!bIsActive) return;

  if (!bIsAuthenticating) {
    if (SecondsSinceLastRequest >= SecondsBetweenRequests) {
      RequestLivemap();
      SecondsSinceLastRequest = 0.0;
    }
  }

  auto World = GetWorld();
  auto GeoRefSystem = AGeoReferencingSystem::GetGeoReferencingSystem(World);
  if (GeoRefSystem && Vehicles.Num() > 0)
  {
    for (const auto& Vehicle : Vehicles)
    {
      if (Vehicle.Gid.IsEmpty())
      {
        //I've never seen this warning trigger, but hey let's keep it, might be fun to debug
        UE_LOG(LogSwagger, Warning, TEXT("GOT EMPTY VEHICLE GID"));
        continue;
      }
      if (VehicleActorMap.Contains(Vehicle.Gid)) {
        //Västtrafik API sometimes gives us very large jumps in positional data.
        //we might write code to correct for it, 
        //for now we trust them, but it do look funky, y'know
        AVehicleActor* VehicleActor = VehicleActorMap[Vehicle.Gid];
        VehicleActor->SetNextCoordinates(Vehicle.X, Vehicle.Y);
      }
      else {
        AVehicleActor* NewVehicleActor = nullptr;
        EVehicleActorType VehicleActorType = EVehicleActorType::UNKNOWN;
        switch (Vehicle.ProdClass)
        {
        case Swagger::VehicleType::Bus:
          NewVehicleActor = World->SpawnActor<AVehicleActor>(BusActorType);
          VehicleActorType = EVehicleActorType::BUS;
          break;
        case Swagger::VehicleType::Tram:
          NewVehicleActor = World->SpawnActor<AVehicleActor>(TramActorType);
          VehicleActorType = EVehicleActorType::TRAM;
          break;
          //Long distance trains, regional trains and Västtågen trains
        case Swagger::VehicleType::Ldt: case Swagger::VehicleType::Reg: case Swagger::VehicleType::Vas:
          NewVehicleActor = World->SpawnActor<AVehicleActor>(TrainActorType);
          VehicleActorType = EVehicleActorType::TRAIN;
          break;
        case Swagger::VehicleType::Boat:
          NewVehicleActor = World->SpawnActor<AVehicleActor>(BoatActorType);
          VehicleActorType = EVehicleActorType::BOAT;
          break;
          //I've never seen a "Taxi" type of vehicle in the livemap data, but maybe we should show it?
          //case Swagger::VehicleType::Taxi:
          //  NewVehicleActor = World->SpawnActor<AVehicleActor>(TaxiActorType);
          //  VehicleActorType = EVehicleActorType::TAXI;
          //  break;
        default:
          break;
        }
        if (NewVehicleActor) {
          FColor LineColor = FColor::FromHex(Vehicle.Lcolor);
          FColor BgColor = FColor::FromHex(Vehicle.Bcolor);

          NewVehicleActor->Gid = Vehicle.Gid;
          NewVehicleActor->Type = VehicleActorType;
          NewVehicleActor->SetInstigator(this);
          NewVehicleActor->SetColor(LineColor.ReinterpretAsLinear());
          NewVehicleActor->SetBackgroundColor(BgColor.ReinterpretAsLinear());
          NewVehicleActor->SetJourneyName(Vehicle.Name);
          NewVehicleActor->SetNextCoordinates(Vehicle.X, Vehicle.Y);

          if (EnumHasAnyFlags(EVehicleActorType(ActiveVehicleTypes), VehicleActorType) == false) {
            NewVehicleActor->SetActorHiddenInGame(true);
          }
          VehicleActorMap.Add(Vehicle.Gid, NewVehicleActor);
        }
      }
    }

    Vehicles.Empty();
  }

  SecondsSinceLastRequest += DeltaTime;
}

bool ATrafficActor::Authenticate()
{
  UE_LOG(LogSwagger, Log, TEXT("Authenticating to Västtrafik!"));
  bIsAuthenticating = true;
  const FString ClientGrantString = "grant_type=client_credentials&scope=" + DEVICE_ID;

  auto AuthRequest = FHttpModule::Get().CreateRequest();

  AuthRequest->SetTimeout(1.f);
  AuthRequest->SetVerb("POST");
  AuthRequest->SetHeader("Content-Type", "application/x-www-form-urlencoded");
  //TODO: These keys should be kept secret, hushhh!
  //This is an oauth2 basic authorization request header.
  AuthRequest->SetHeader("Authorization", "Basic bUk4MkVNWVo1X0pLX3p5Vml2djFmZHRKdFFrYTpBQXh3V01YY3JuR01UZ29YTFNLZ21hSjFtSklh");

  AuthRequest->SetURL(APITokenURL);
  AuthRequest->SetContentAsString(ClientGrantString);

  AuthRequest->OnProcessRequestComplete().BindUObject(this, &ATrafficActor::OnAuthFinished);

  return AuthRequest->ProcessRequest();
}

void ATrafficActor::RequestLivemap()
{
  const FString AuthHeader = "Bearer " + Authentication.access_token;

  Swagger::SwaggerLivemapApi::LivemapRequest Request;

  //TODO: Make the request extent modifyable
  Request.Minx = "11934405";
  Request.Maxx = "12000305";
  Request.Miny = "57683101";
  Request.Maxy = "57730181";
  //INVESTIGATE: Should we only request realtime?
  Request.OnlyRealtime = Swagger::SwaggerLivemapApi::LivemapRequest::OnlyRealtimeEnum::Yes;

  Swagger::SwaggerLivemapApi LivemapApi;
  LivemapApi.SetURL(APIBasePath);
  LivemapApi.AddHeaderParam("Authorization", AuthHeader);
  LivemapApi.Livemap(Request, Delegate);
}

void ATrafficActor::OnAuthFinished(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
  const int32 ResponseCode = Response->GetResponseCode();
  if (ResponseCode == 200)
  {
    FString ResponseString = Response->GetContentAsString();

    if (!FJsonObjectConverter::JsonObjectStringToUStruct(ResponseString, &Authentication, 0, 0)) {
      UE_LOG(LogSwagger, Error, TEXT("Http auth response returned unknown data: %s"), *Response->GetContentAsString());
    }
  }
  else {
    UE_LOG(LogSwagger, Error, TEXT("Http request failed, error code: %d"), ResponseCode);
  }

  bIsAuthenticating = false;
}


void ATrafficActor::OnLiveMapResponse(Swagger::SwaggerLivemapApi::LivemapResponse& Response)
{
  const int32 ResponseCode = Response.GetHttpResponseCode();
  if (ResponseCode == 200)
  {
    //Only accept newer responses than the last one
    if (Response.Content.Time > LastLivemapUpdate) {
      LastLivemapUpdate = Response.Content.Time;
      Swap(Vehicles, Response.Content.Vehicles);
    }
  }
  else {
    UE_LOG(LogSwagger, Error, TEXT("Http request failed, error code: %d"), ResponseCode);
  }

}

void ATrafficActor::RequestRemoval(AVehicleActor* Vehicle)
{
  VehicleActorMap.Remove(Vehicle->Gid);
  Vehicle->Destroy();
}

void ATrafficActor::ToggleTraffic(bool bActiveState)
{
  bIsActive = bActiveState;
  if (!bIsActive) {
    for (auto& Vehicle : VehicleActorMap)
    {
      Vehicle.Value->Destroy();
    }
    VehicleActorMap.Empty();
  }
}

void ATrafficActor::ActivateVehicleType(EVehicleActorType VehicleActorType)
{
  const bool bHasFlag = EnumHasAnyFlags((EVehicleActorType)ActiveVehicleTypes, VehicleActorType);
  if (!bHasFlag) {
    EnumAddFlags((EVehicleActorType&)ActiveVehicleTypes, VehicleActorType);

    for (auto& Vehicle : VehicleActorMap)
    {
      if (Vehicle.Value->Type == VehicleActorType) {
        //Since "Hidden" is the inverse of active, we can use the previous flag here.
        Vehicle.Value->SetActorHiddenInGame(bHasFlag);
      }
    }
  }
}

void ATrafficActor::DeactivateVehicleType(EVehicleActorType VehicleActorType)
{
  const bool bHasFlag = EnumHasAnyFlags((EVehicleActorType)ActiveVehicleTypes, VehicleActorType);
  if (bHasFlag) {
    EnumRemoveFlags((EVehicleActorType&)ActiveVehicleTypes, VehicleActorType);

    for (auto& Vehicle : VehicleActorMap)
    {
      if (Vehicle.Value->Type == VehicleActorType) {
        //Since "Hidden" is the inverse of active, we can use the previous flag here.
        Vehicle.Value->SetActorHiddenInGame(bHasFlag);
      }
    }
  }
}

void ATrafficActor::ToggleVehicleType(EVehicleActorType VehicleActorType)
{
  const bool bHasFlag = EnumHasAnyFlags((EVehicleActorType)ActiveVehicleTypes, VehicleActorType);
  if (bHasFlag) {
    EnumRemoveFlags((EVehicleActorType&)ActiveVehicleTypes, VehicleActorType);
  }
  else {
    EnumAddFlags((EVehicleActorType&)ActiveVehicleTypes, VehicleActorType);
  }
  for (auto& Vehicle : VehicleActorMap)
  {
    if (Vehicle.Value->Type == VehicleActorType) {
      //Since "Hidden" is the inverse of active, we can use the previous flag here.
      Vehicle.Value->SetActorHiddenInGame(bHasFlag);
    }
  }
}
