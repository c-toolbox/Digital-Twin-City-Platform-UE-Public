// Fill out your copyright notice in the Description page of Project Settings.
#include "DatasetCatalog.h"

URasterDataset* CreateRasterDataset(IFileManager& FileManager, const FString& CatalogDir, const FString& DatasetDir)
{
  URasterDataset* Raster = NewObject<URasterDataset>();
  Raster->SetDatasetName(FPaths::GetCleanFilename(DatasetDir));

  //Find image file in the Dataset folder, assume the first one is the raster:
  TArray<FString> PngFiles;
  FileManager.FindFiles(PngFiles, *DatasetDir, TEXT("png"));
  if (PngFiles.Num()) {
    Raster->SetImageFilePath(DatasetDir / PngFiles[0]);
  }
  return Raster;
}

void UDatasetCatalog::InitializeCatalog(IFileManager& FileManager, const FString& CatalogDirectory)
{
  CatalogName = FPaths::GetCleanFilename(CatalogDirectory);

  //Find all directories in the Catalog and assume they are RasterDatasets:
  TArray<FString> DatasetNames;
  FileManager.FindFiles(DatasetNames, *(CatalogDirectory / "*"), false, true);

  //Create one Package per Catalog 
  //This is each .uasset-file on the filesystem
  const FString PackageName = "/Game/DatasetCache" / CatalogName;
  UPackage* Package = CreatePackage(*PackageName);

  for (const FString& DatasetName : DatasetNames)
  {
    FString DatasetDirectory = CatalogDirectory / DatasetName;
    URasterDataset* Raster = CreateRasterDataset(FileManager, CatalogDirectory, DatasetDirectory);
    if (!Raster->TryLoadFromCache(Package, PackageName))
    {
      TArray<FString> JsonFiles;
      FileManager.FindFiles(JsonFiles, *DatasetDirectory, TEXT("json"));
      if (JsonFiles.Num())
      {
        Raster->SetMetaData(FRasterDataRow::ParseFromJsonFile(DatasetDirectory / JsonFiles[0]));
      }
      Raster->LoadTextureFromImageFile(Package, PackageName);
    }
    Datasets.Push(Raster);
  }
}

bool UDatasetCatalog::ContainsDataset(const FString& DatasetName) const
{
  for (UDataset* Dataset : Datasets)
  {
    if (Dataset->GetDatasetName() == DatasetName)
    {
      return true;
    }
  }
  return false;
}

bool UDatasetCatalog::ActivateDataset(UWorld* World, const FString& DatasetName)
{
  for (UDataset* Dataset : Datasets)
  {
    if (Dataset->GetDatasetName() == DatasetName)
    {
      Dataset->ActivateDataset(World);
      return true;
    }
  }
  return false;
}

bool UDatasetCatalog::ToggleDataset(UWorld* World, const FString& DatasetName)
{
  for (UDataset* Dataset : Datasets)
  {
    if (Dataset->GetDatasetName() == DatasetName)
    {
      Dataset->ToggleDataset(World);
      return true;
    }
  }
  return false;
}

TArray<FString> UDatasetCatalog::GetDatasetNames() const
{
  TArray<FString> DatasetNames;
  for (auto Dataset : Datasets)
  {
    DatasetNames.Push(Dataset->GetDatasetName());
  }
  return DatasetNames;
}

void UDatasetCatalog::AddDataset(UDataset* Dataset)
{
  Datasets.Push(Dataset);
}

void UDatasetCatalog::DisableAllDatasets(UWorld* World)
{
  for (auto& Dataset : Datasets)
  {
    Dataset->DisableDataset(World);
  }
}