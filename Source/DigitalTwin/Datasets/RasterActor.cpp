// Fill out your copyright notice in the Description page of Project Settings.
#include "RasterActor.h"
#include "GeoReferencingSystem.h"

#include "Components/DecalComponent.h" 

//Gothenburg is no higher than this!
const float DECAL_HEIGHT = 500.0;

ARasterActor::ARasterActor()
  :Super()
{
  DecalComponent = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComponent"));
  DecalComponent->SetRelativeRotation(FRotator(-90, 0, 0));
  DecalComponent->bDestroyOwnerAfterFade = true;

  RootComponent = DecalComponent;
  RootComponent = DecalComponent;
  ConstructorHelpers::FObjectFinder<UMaterial> DecalMaterial(TEXT("/Game/Materials/DecalMaterial"));
  if (DecalMaterial.Succeeded()) {
    DecalComponent->SetDecalMaterial(DecalMaterial.Object);
  }
  DynamicMaterial = nullptr;

  DynamicMaterial = nullptr;
}

void ARasterActor::SetMaterialPath(const FString &MaterialPath) {

  UE_LOG(LogTemp, Warning, TEXT("ARasterActor::SetMaterialPath"));
  // 1. Check path !

  if (UMaterial *UnrealMaterial = Cast<UMaterial>(
          StaticLoadObject(UMaterial::StaticClass(), nullptr, *MaterialPath))) {
    DecalComponent->SetDecalMaterial(UnrealMaterial);
  }

  /*
  if (RootComponent->MarkPackageDirty()) {
    UE_LOG(LogTemp, Warning, TEXT("RootComponent is marked dirty"));
  } else {
    UE_LOG(LogTemp, Warning, TEXT("RootComponent marked dirty failed !"));
  }
  */
}


void ARasterActor::OnConstruction(const FTransform& Transform)
{
  Super::OnConstruction(Transform);
  DecalComponent->DecalSize = FVector(DECAL_HEIGHT, YExtent / 2.0, XExtent / 2.0);
}

void ARasterActor::BeginPlay()
{
  Super::BeginPlay();
}

void ARasterActor::SetDecalTexture(UTexture* Texture,int32 SortOrder)
{
  DecalTexture = Texture;
  if (!DynamicMaterial) {
    DynamicMaterial = DecalComponent->CreateDynamicMaterialInstance();
  }
  if (DecalTexture) {
    DynamicMaterial->SetTextureParameterValue(TEXT("Texture"), DecalTexture);
    DecalComponent->SetDecalMaterial(DynamicMaterial);
    DecalComponent->SortOrder = SortOrder;
  }
}

void ARasterActor::SetExtent(const FExtent& NewExtent)
{
  auto World = GetWorld();
  check(World);
  auto GeoRefSystem = AGeoReferencingSystem::GetGeoReferencingSystem(World);
  if (GeoRefSystem)
  {
    XExtent = NewExtent.Max.X - NewExtent.Min.X;
    YExtent = NewExtent.Max.Y - NewExtent.Min.Y;
    
    FCartesianCoordinates ExtentCenter(NewExtent.Min.X + XExtent / 2.0, NewExtent.Min.Y + YExtent / 2.0, 0);
    FGeographicCoordinates Coords;
    GeoRefSystem->ProjectedToGeographic(ExtentCenter, Coords);
    Wgs84Latitude  = Coords.Latitude;
    Wgs84Longitude = Coords.Longitude;
    DecalComponent->DecalSize = FVector(DECAL_HEIGHT, YExtent / 2.0,XExtent / 2.0);
    DecalComponent->RecreateRenderState_Concurrent();
  }
}

#if WITH_EDITOR
void ARasterActor::PostEditMove(bool bFinished)
{
  Super::PostEditMove(bFinished);

  DecalComponent->RecreateRenderState_Concurrent();
}

void ARasterActor::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
  // AActor::PostEditChange will ForceUpdateComponents()
  Super::PostEditChangeProperty(PropertyChangedEvent);

  DecalComponent->RecreateRenderState_Concurrent();
}
#endif //WITH_EDITOR