// Fill out your copyright notice in the Description page of Project Settings.

#include "MediaDataset.h"
#include "RasterActor.h"
#include "Components/DecalComponent.h"

#include "LatentActions.h"
#include "MediaTexture.h"
#include "MediaPlayer.h"
#include "MediaSource.h"

UMediaDataset::UMediaDataset()
  : Super(), RasterActor(nullptr)
{
  ConstructorHelpers::FObjectFinder<UMaterial> FoundDecalMaterial(TEXT("/Game/Materials/MovieDecalMaterial"));
  if (FoundDecalMaterial.Succeeded()) {
    MediaDecalMaterial = FoundDecalMaterial.Object;
  }
}

static int32 UUID = 0;
void UMediaDataset::ActivateDataset(UWorld* World)
{
  if (!RasterActor) {
    if (MetaData.MediaPlayer) {
      FLatentActionInfo LatentInfo;
      LatentInfo.CallbackTarget = this;
      LatentInfo.ExecutionFunction = "PlaybackResumed";
      //"UUID" probably doesn't matter too much, but Linkage needs to be set to 0
      LatentInfo.UUID = UUID++;
      LatentInfo.Linkage = 0;
      bool bSuccess;
      FMediaPlayerOptions Options;
      MetaData.MediaPlayer->OpenSourceLatent(World, LatentInfo, MetaData.MediaSource, Options, bSuccess);
    }
  }
}

void UMediaDataset::ToggleDataset(UWorld* World)
{
  if (!RasterActor) {
    ActivateDataset(World);
  }
  else {
    RasterActor->Destroy();
    RasterActor = nullptr;
  }
}

void UMediaDataset::DisableDataset(UWorld* World) {
  if (RasterActor) {
    ToggleDataset(World);
  }
}

inline void UMediaDataset::SetTexture(UMediaTexture* InTexture)
{
  MediaTexture = InTexture;
}

void UMediaDataset::SetMetaData(const FMediaDataRow& InMetadata)
{
  MetaData = InMetadata;
  if (InMetadata.MediaTexture) {
    MediaTexture = InMetadata.MediaTexture;
  }
  SetDatasetName(MetaData.Name);
}

UMediaTexture* UMediaDataset::GetMediaTexture() const
{
  return MediaTexture;
}

void UMediaDataset::PlaybackResumed()
{
  auto World = GetWorld();
  if (!RasterActor && World) {
    const FExtent& Extent = MetaData.Extent;
    if (World->WorldType == EWorldType::Game || World->WorldType == EWorldType::GamePreview || World->WorldType == EWorldType::PIE)
    {
      RasterActor = World->SpawnActor<ARasterActor>();
      //HACK: In order to use RasterActor with mediatextures, we 
      //overwrite the existing decalmaterial with a 
      //media decal material.
      RasterActor->DecalComponent->SetDecalMaterial(MediaDecalMaterial);
#if WITH_EDITORONLY_DATA
      RasterActor->SetActorLabel(DatasetName);
#endif
      RasterActor->DatasetName = DatasetName;
      RasterActor->SetDecalTexture(MediaTexture);
      RasterActor->SetExtent(Extent);
      RasterActor->AddActorLocalRotation(MetaData.Rotation);
    }
  }
}
