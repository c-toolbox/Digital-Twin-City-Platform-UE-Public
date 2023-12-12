// Fill out your copyright notice in the Description page of Project Settings.


#include "DigitalTwin/Datasets/DatasetSubsystem.h"
#include "DigitalTwin/GeoUtils/GeoActor.h"
#include "Metadata.h"
#include "GeographicCoordinates.h"
#include "GeoReferencingSystem.h"
#include "RasterActor.h"
#include "MediaDataset.h"

#include "Components/TextRenderComponent.h"
#include "Engine/TextRenderActor.h"
#include "Kismet/GameplayStatics.h"



FString FDatasetPaths::DatasetRootDir() {
  FString DatasetDir = FPaths::ProjectDir() + TEXT("Datasets");
  if (!FPaths::DirectoryExists(DatasetDir)) {
    UE_LOG(LogTemp, Warning, TEXT("Could not find Dataset root dir: %s, creating it..."), *DatasetDir);
    IFileManager::Get().MakeDirectory(*DatasetDir);
  }
  return DatasetDir;
}


TArray<FString> GetDirectories(IFileManager& FileManager, const FString& FilePath) {
  TArray<FString> FoundDirectories;
  FString SearchPath = FilePath / "*";
  FileManager.FindFiles(FoundDirectories, *SearchPath, false, true);
  return FoundDirectories;
}

void UDatasetSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
  Super::Initialize(Collection);

  UWorld* World = GetWorld();
  if (World && World->WorldType == EWorldType::Game || World->WorldType == EWorldType::GamePreview || World->WorldType == EWorldType::PIE)
  {
    IFileManager& FileManager = IFileManager::Get();
    FString DatasetRootDir = FDatasetPaths::DatasetRootDir();
    UE_LOG(LogTemp, Warning, TEXT("Dataset root dir: %s"), *DatasetRootDir);
    TArray<FString> CatalogDirectories =  GetDirectories(FileManager, DatasetRootDir);
    for (const FString& CatalogName : CatalogDirectories) {
      FString CatalogDir = DatasetRootDir / CatalogName;
      UE_LOG(LogTemp, Warning, TEXT("Dataset root dir: %s"), *DatasetRootDir);
      UDatasetCatalog* Catalog = NewObject<UDatasetCatalog>(this);
      Catalog->InitializeCatalog(FileManager, CatalogDir);
      DatasetCatalogs.Push(Catalog);
    }
  }
}

void UDatasetSubsystem::Deinitialize()
{
  Super::Deinitialize();
}

void UDatasetSubsystem::RegisterRasterDataset(UDataTable* RasterData)
{
  UDatasetCatalog* NewCatalog = NewObject<UDatasetCatalog>(this);
  FString DataName = RasterData->GetName();
  DataName.RemoveFromEnd("Data");
  NewCatalog->SetCatalogName(*DataName);

  TArray<FRasterDataRow*> RasterMetadatas;
  RasterData->GetAllRows<FRasterDataRow>("Raster Metadata", RasterMetadatas);
  for (const auto& Metadata : RasterMetadatas) {
    URasterDataset* RasterDataset = NewObject<URasterDataset>(this);
    RasterDataset->SetMetaData(*Metadata);
    NewCatalog->AddDataset(RasterDataset);
  }
  
  RegisterCatalog(NewCatalog);
}

void UDatasetSubsystem::RegisterMovieDataset(UDataTable* MovieData)
{
  UDatasetCatalog* NewCatalog = NewObject<UDatasetCatalog>(this);
  FString DataName = MovieData->GetName();
  DataName.RemoveFromEnd("Data");
  NewCatalog->SetCatalogName(*DataName);

  TArray<FMediaDataRow*> MediaMetadatas;
  MovieData->GetAllRows<FMediaDataRow>("Media Metadata", MediaMetadatas);
  for (const auto& Metadata : MediaMetadatas) {
    UMediaDataset* MediaDataset = NewObject<UMediaDataset>(this);
    MediaDataset->SetMetaData(*Metadata);
    NewCatalog->AddDataset(MediaDataset);
  }

  RegisterCatalog(NewCatalog);
}


void UDatasetSubsystem::RegisterCatalog(UDatasetCatalog* Catalog)
{
  //TODO: Register a dataset !
  UE_LOG(LogTemp, Log, TEXT("Registering %s"), *Catalog->GetCatalogName());
  DatasetCatalogs.Push(Catalog);
}


void UDatasetSubsystem::ActivateDataset(const FString& CatalogName, const FString& DatasetName, int32 SortOrder)
{
  auto World = GetWorld();

  bool bFoundDataset = false;
  for (const auto Catalog : DatasetCatalogs) {
    if (Catalog->GetCatalogName() == CatalogName) {
      //TODO: Might call this "Activate" instead...
      if (Catalog->ActivateDataset(World, DatasetName,SortOrder)) {
        bFoundDataset = true;
        break;
      }
    }
  }
  if (!bFoundDataset) {
    UE_LOG(LogTemp, Warning, TEXT("Could not find catalog - dataset: %s - %s"), *CatalogName, *DatasetName);
  }
  else {
    UE_LOG(LogTemp, Log, TEXT("Activated dataset: %s - %s"), *CatalogName, *DatasetName);
  }
}

UTexture2D* UDatasetSubsystem::ActivateDataset2(const FString &CatalogName,
                                         const FString &DatasetName) {
  
  // 1. Find folder/catalog ...
  const auto FolderName = FDatasetPaths::DatasetRootDir() + "/" + CatalogName;
  if (FPaths::DirectoryExists(FolderName)) {
    UE_LOG(LogTemp, Warning, TEXT("Catalog exists : %s "), *CatalogName);
  } else {
    UE_LOG(LogTemp, Warning, TEXT("Catalog dosen`t exists !"));
    return nullptr;
  }

  // 2. Check if images exists ...
  const auto FileName = FolderName + "/" + DatasetName;
  if(FPaths::FileExists(FileName)) {
    UE_LOG(LogTemp, Warning, TEXT("File %s ,exists i Filedatabase !"), *DatasetName);
  } else {
    UE_LOG(LogTemp, Warning, TEXT("File dosen`t exists !"));
    return nullptr;
  }
  
  // 3. Read image and create texture !
  // 4. Read meta data
  
  FImageData ImageData;
  ImageData.ReadImageData(FileName);

  const auto RasterTexture = UTexture2D::CreateTransient(ImageData.Width, ImageData.Height, ImageData.PixelFormat);
  const auto Mip = &RasterTexture->PlatformData->Mips[0];
 
  void* TextureData = RasterTexture->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
  FMemory::Memcpy(TextureData, ImageData.RawPixelData.GetData(), ImageData.RawPixelData.Num());
  RasterTexture->PlatformData->Mips[0].BulkData.Unlock();  
  RasterTexture->UpdateResource();

  //FEvent Extent;
  
  RasterActor = GetWorld()->SpawnActor<ARasterActor>();

  RasterActor->DatasetName = DatasetName;
  RasterActor->SetDecalTexture(RasterTexture,0);
  //RasterActor->SetExtent(Extent);

  
  return RasterTexture;
  
}

bool UDatasetSubsystem::CheckDatasetCatalog(UDatasetCatalog* Catalog) {
  for(const auto c : DatasetCatalogs) {
    //
    if(Catalog->GetCatalogName() == c->GetCatalogName()) {
       return true;
    }
  }
  return false;
}


void UDatasetSubsystem::DisableAllDatasets()
{
  for (auto& Catalog : DatasetCatalogs) {
    Catalog->DisableAllDatasets(GetWorld());
  }
}

TArray<FString> UDatasetSubsystem::GetCatalogNames() const {
  TArray<FString> CatalogNames;
  for (const auto &Catalog : DatasetCatalogs) {
    CatalogNames.AddUnique(Catalog->GetCatalogName());
  }
  return CatalogNames;
}

TArray<FString> UDatasetSubsystem::GetDatasetNames(const FString& CatalogName) const
{
  TArray<FString> DatasetNames;
  for (const auto& Catalog : DatasetCatalogs)
  {
    if (Catalog->GetCatalogName() == CatalogName) {
      return Catalog->GetDatasetNames();
    }
  }

  return DatasetNames;
}

//Used for splitting a string into lines of LineLength. 
FString SplitLines(const FString& InString, int32 LineLength = 50)
{
  FString OutString;
  TArray<FString> StringLines;
  InString.ParseIntoArrayLines(StringLines);
  for (auto& Line : StringLines) {
    if (Line.Len() > LineLength) {
      TArray<FString> Words;
      Line.ParseIntoArrayWS(Words);
      int32 LineLen = 0;
      for (auto& Word : Words) {
        int32 AddedLen = Word.Len() + 1;
        if (LineLen + AddedLen > LineLength) {
          OutString.AppendChar('\n');
          LineLen = 0;
        }
        OutString.Append(Word);
        OutString.AppendChar(' ');
        LineLen += AddedLen;
      }
    }
    else {
      OutString.Append(Line);
      OutString.AppendChar('\n');
    }
  }

  return OutString;
}

void UDatasetSubsystem::SetText1(const FString& InString)
{
  TArray<AActor*> AllActors;
  UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATextRenderActor::StaticClass(), AllActors);

  FString SplitString = SplitLines(InString);
  for (auto Actor : AllActors) {
    FString ActorName = Actor->GetName();

    if (ActorName.Contains("Text1"))
    {
      auto TextActor = Cast<ATextRenderActor>(Actor);
      TextActor->GetTextRender()->SetText(FText::FromString(SplitString));
    }
  }
}

void UDatasetSubsystem::SetText2(const FString& InString)
{
  TArray<AActor*> AllActors;
  UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATextRenderActor::StaticClass(), AllActors);

  FString SplitString = SplitLines(InString);
  for (auto Actor : AllActors) {
    FString ActorName = Actor->GetName();

    if (ActorName.Contains("Text2"))
    {
      auto TextActor = Cast<ATextRenderActor>(Actor);
      TextActor->GetTextRender()->SetText(FText::FromString(SplitString));
    }
  }
}
