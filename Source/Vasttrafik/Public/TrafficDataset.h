
#pragma once

#include "CoreMinimal.h"
#include "DigitalTwin/Datasets/Dataset.h"
#include "VehicleActor.h"
#include "TrafficDataset.generated.h"


/**
* UTrafficDataset - Given a TrafficActor and VehicleType, can toggle v�sttrafik traffic
*/
UCLASS()
class VASTTRAFIK_API UTrafficDataset : public UDataset
{
  GENERATED_BODY()
public:
  virtual void ActivateDataset(UWorld* World,int32 SortOrder, FString MaterialPath) override;
  virtual void ToggleDataset(UWorld* World) override;
  virtual void DisableDataset(UWorld* World) override;

  UPROPERTY()
    class ATrafficActor* TrafficActor;

  UPROPERTY()
    EVehicleActorType VehicleType;
private:
};

