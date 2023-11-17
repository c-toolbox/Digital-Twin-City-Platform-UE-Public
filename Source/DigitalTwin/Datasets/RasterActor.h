// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RasterDataset.h"
#include "Metadata.h"
#include "../GeoUtils/GeoActor.h"


//TODO: Remove once we move the actor?
#include "RasterActor.generated.h"

UCLASS()
class DIGITALTWIN_API ARasterActor : public AGeoActor
{
  GENERATED_BODY()
public:
  ARasterActor();
  
  virtual void OnConstruction(const FTransform& Transform) override;

  void BeginPlay() override;
  UFUNCTION(BlueprintCallable, Category = "DigitalTwin|RasterActor")
  void SetDecalTexture(class UTexture* Texture);

  UFUNCTION(BlueprintCallable, Category = "DigitalTwin|RasterActor")
  void SetExtent(const FExtent& NewExtent);

#if WITH_EDITOR
  //~ Begin UObject Interface
  virtual void PostEditMove(bool bFinished) override;
  virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
  //~ End UObject Interface
#endif //WITH_EDITOR

  UPROPERTY(EditAnywhere)
    class UDecalComponent* DecalComponent;

  UPROPERTY()
    class UMaterialInstanceDynamic* DynamicMaterial;

  UPROPERTY(EditAnywhere)
    class UTexture* DecalTexture;

  UPROPERTY(EditAnywhere)
    FString DatasetName;

protected:
  UPROPERTY(EditAnywhere)
    double XExtent = 100.0;
  UPROPERTY(EditAnywhere)
    double YExtent = 100.0;
};
