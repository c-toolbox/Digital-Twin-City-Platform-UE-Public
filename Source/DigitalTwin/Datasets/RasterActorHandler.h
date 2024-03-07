#pragma once

#include "CoreMinimal.h"
#include "RasterActorHandler.generated.h"

// Purpose of this class is to spawn RasterActors
UCLASS()
class ARasterActorHandler : public AActor{
  GENERATED_BODY()
public:
  ARasterActorHandler();
  // Called every frame
  virtual void Tick(float DeltaTime) override;
  virtual void OnConstruction(const FTransform& Transform) override;
  //void AddRaster(UTexture)
protected:
  // Called when the game starts or when spawned
  virtual void BeginPlay() override;
};
