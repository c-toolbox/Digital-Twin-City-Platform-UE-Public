
#pragma once

#include "CoreMinimal.h"

#include "Dataset.generated.h"

/**
* UDataset - Base class for Datasets
*/
UCLASS()
class DIGITALTWIN_API UDataset : public UObject
{
  GENERATED_BODY()
public:
  virtual void ActivateDataset(UWorld* World,int32 SortOrder) {}
  virtual void ToggleDataset(UWorld* World) {}
  virtual void DisableDataset(UWorld* World) {}

  void SetDatasetName(const FString& InDatasetName)
  {
    DatasetName = InDatasetName;
  }

  const FString& GetDatasetName() const
  {
    return DatasetName;
  }

protected:

  FString DatasetName;
};

