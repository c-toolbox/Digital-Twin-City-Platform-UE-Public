// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "DatasetCatalog.h"
#include "DatasetSubsystem.generated.h"


class DIGITALTWIN_API FDatasetPaths {
public:
  static FString DatasetRootDir();
};

/**
 * The DatasetSubsystem handles registration of datasets.
 * This Subsystem can be seen as the container of dataset content.
 */
UCLASS()
class DIGITALTWIN_API UDatasetSubsystem : public UWorldSubsystem
{
  GENERATED_BODY()

public:

  void Initialize(FSubsystemCollectionBase& Collection) override;
  void Deinitialize() override;

  UFUNCTION(BlueprintCallable, Category = "DigitalTwin|Datasets")
    void RegisterRasterDataset(UDataTable* RasterData);

  UFUNCTION(BlueprintCallable, Category = "DigitalTwin|Datasets")
    void RegisterMovieDataset(UDataTable* MovieData);

  void RegisterCatalog(class UDatasetCatalog* Catalog);

  UFUNCTION(BlueprintCallable, Category = "DigitalTwin|Datasets")
    void ActivateDataset(const FString& CatalogName, const FString& DatasetName);

  UFUNCTION(BlueprintCallable, Category = "DigitalTwin|Datasets")
    void DisableAllDatasets();

  UFUNCTION(BlueprintCallable, Category = "DigitalTwin|Datasets")
    TArray<FString> GetCatalogNames() const;

  UFUNCTION(BlueprintCallable, Category = "DigitalTwin|Datasets")
    TArray<FString> GetDatasetNames(const FString& CatalogName) const;

  //TODO: This does not really belong here...
  UFUNCTION(BlueprintCallable, Category = "DigitalTwin|Datasets")
    void SetText1(const FString& InString);

  UFUNCTION(BlueprintCallable, Category = "DigitalTwin|Datasets")
    void SetText2(const FString& InString);

private:

  UPROPERTY()
    TArray<class UDatasetCatalog*> DatasetCatalogs;
};
