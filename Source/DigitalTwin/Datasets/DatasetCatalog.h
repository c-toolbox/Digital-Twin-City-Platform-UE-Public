// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RasterDataset.h"
#include "DatasetCatalog.generated.h"

//A DatasetCatalog is a top-level catalogue for different datasets
//e.g. basemaps could be a datasetcatalogue
//Typical folder structure looks like:
// - Datasets
//   - Basemaps (Catalog)
//     - Ortho_2019 (RasterDataset)
//     - Citymap (RasterDataset)
//   - Temperature (Catalog)
//     - Temperature_2020 (RasterDataset)
//   ... and so on
UCLASS()
class DIGITALTWIN_API UDatasetCatalog : public UObject
{
  GENERATED_BODY()
public:
  /*
  * Initializes this Catalog.
  * Searches the filesystem for datasets given a CatalogDirectory.
  */
  void InitializeCatalog(IFileManager& FileManager, const FString& CatalogDirectory);

  /*
  * Searches the Datasets in this Catalog for `DatasetName` and returns true if found
  * returns false if no such dataset was found.
  */
  bool ContainsDataset(const FString& DatasetName) const;

  bool ActivateDataset(UWorld* World, const FString& DatasetName);

  bool ToggleDataset(UWorld* World, const FString& DatasetName);

  const FString& GetCatalogName() const
  {
    return CatalogName;
  }

  /*
  * Gets the Dataset names in this Catalog.
  */
  TArray<FString> GetDatasetNames() const;

  void SetCatalogName(const FString& InCatalogName)
  {
    CatalogName = InCatalogName;
  }

  void AddDataset(class UDataset* Dataset);

  void DisableAllDatasets(UWorld* World);

private:

  FString CatalogName;

  UPROPERTY()
    TArray<class UDataset*> Datasets;
};
