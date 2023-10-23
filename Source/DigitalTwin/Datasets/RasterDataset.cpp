// Fill out your copyright notice in the Description page of Project Settings.

#include "RasterDataset.h"
#include "GenericPlatform/GenericPlatformFile.h"
#include "RasterActor.h"



void FImageData::ReadImageData(const FString& ImageFilePath)
{
  IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
  TArray64<uint8> CompressedImageData;
  if (!FFileHelper::LoadFileToArray(CompressedImageData, *ImageFilePath))
  {
    UE_LOG(LogTemp, Warning, TEXT("Could not load file: %s"), *ImageFilePath);
    return;
  }

  if (ImageFilePath.EndsWith(".png"))
  {
    UE_LOG(LogTemp, Warning, TEXT("Parsing PNG image"));
    ParsePNG(ImageWrapperModule, CompressedImageData);
  }
}

void FImageData::ParsePNG(IImageWrapperModule& ImageWrapperModule, const TArray64<uint8>& CompressedImageData)
{
  const uint8* Buffer = CompressedImageData.GetData();
  const uint64 BufLen = CompressedImageData.Num();
  TSharedPtr<IImageWrapper> PngImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);
  if (PngImageWrapper.IsValid() && PngImageWrapper->SetCompressed(Buffer, BufLen))
  {
    //TODO: Ensure we are picking valid image formats
    TextureFormat = TSF_Invalid;
    BitDepth = PngImageWrapper->GetBitDepth();
    Format = PngImageWrapper->GetFormat();
    PixelFormat = EPixelFormat::PF_Unknown;
    if (Format == ERGBFormat::Gray)
    {
      if (BitDepth <= 8)
      {
        TextureFormat = TSF_G8;
        PixelFormat = PF_G8;
        Format = ERGBFormat::Gray;
        BitDepth = 8;
      }
      else if (BitDepth == 16)
      {
        TextureFormat = TSF_G16;
        PixelFormat = PF_G16;
        Format = ERGBFormat::Gray;
        BitDepth = 16;
      }
    }
    else if (Format == ERGBFormat::RGBA || Format == ERGBFormat::BGRA)
    {
      if (BitDepth <= 8)
      {
        TextureFormat = TSF_BGRA8;
        Format = ERGBFormat::BGRA;
        PixelFormat = PF_B8G8R8A8;
        BitDepth = 8;
      }
      else if (BitDepth == 16)
      {
        TextureFormat = TSF_RGBA16;
        Format = ERGBFormat::RGBA;
        PixelFormat = PF_R16G16B16A16_UINT;
        BitDepth = 16;
      }
    }

    if (TextureFormat == TSF_Invalid)
    {
      UE_LOG(LogTemp, Error, TEXT("PNG file contains data in an unsupported format."));
    }

    if (PngImageWrapper->GetRaw(Format, BitDepth, RawPixelData))
    {
      Height = PngImageWrapper->GetHeight();
      Width = PngImageWrapper->GetWidth();
    }
  }
  else
  {
    UE_LOG(LogTemp, Error, TEXT("PNG file contains unexpected data!"));
  }
}

void URasterDataset::ActivateDataset(UWorld* World)
{
  if (!RasterActor) {
    const FExtent& Extent = MetaData.Extent;

    if (World->WorldType == EWorldType::Game || World->WorldType == EWorldType::GamePreview || World->WorldType == EWorldType::PIE)
    {
      RasterActor = World->SpawnActor<ARasterActor>();
#if WITH_EDITORONLY_DATA
      RasterActor->SetActorLabel(DatasetName);
#endif
      RasterActor->DatasetName = DatasetName;
      RasterActor->SetDecalTexture(RasterTexture);
      RasterActor->SetExtent(Extent);
    }
  }
}

void URasterDataset::ToggleDataset(UWorld* World)
{
  if (!RasterActor) {
    ActivateDataset(World);
  }
  else {
    RasterActor->Destroy();
    RasterActor = nullptr;
  }
}

void URasterDataset::DisableDataset(UWorld* World) {
  if (RasterActor) {
    ToggleDataset(World);
  }
}

UTexture2D* URasterDataset::GetRasterTexture() const
{
  return RasterTexture;
}

bool URasterDataset::TryLoadFromCache(UPackage* Package, const FString& PackageName)
{
  FString AssetName = DatasetName;
  FString DataTableAssetName = FPackageName::GetShortName(PackageName) + "Data";

  AssetName.RemoveSpacesInline();
  RasterTexture = LoadObject<UTexture2D>(Package, *AssetName);
  if (RasterTexture)
  {
    UDataTable* DataTable = LoadObject<UDataTable>(Package, *DataTableAssetName);
    if (DataTable) {
      auto Data = DataTable->FindRow<FRasterDataRow>(FName(DatasetName), "Raster MetaData");
      if (Data) {
        MetaData = *Data;
        return true;
      } else {
        return false;
      }
    }
  }
  return false;
}

void URasterDataset::LoadTextureFromImageFile(UPackage* Package, const FString& PackageName)
{
#if WITH_EDITOR
  UE_LOG(LogTemp, Warning, TEXT("Would generate cache!"));
  GenerateTextureCache(Package, PackageName);
#else
  CreateTransientTexture();
#endif
}


#if WITH_EDITOR
void URasterDataset::GenerateTextureCache(UPackage* Package, const FString& PackageName)
{
  //Read image data from file:
  FImageData ImageData;
  ImageData.ReadImageData(ImageFilePath);

  FString TextureAssetName = DatasetName;
  TextureAssetName.RemoveSpacesInline();
  FString DataTableAssetName = FPackageName::GetShortName(PackageName) + "Data";

  RasterTexture = NewObject<UTexture2D>(Package, *TextureAssetName, RF_Public | RF_Standalone);
  if (RasterTexture != NULL)
  {
    RasterTexture->Source.Init(ImageData.Width, ImageData.Height, 1, 1, ImageData.TextureFormat, ImageData.RawPixelData.GetData());
    RasterTexture->SRGB = 1;

    auto IsPowerOfTwo = [](int32 Value)
    {
      return (Value > 0) && ((Value & (Value - 1)) == 0);
    };

    if (!IsPowerOfTwo(ImageData.Width) || !IsPowerOfTwo(ImageData.Height))
    {
      RasterTexture->MipGenSettings = TMGS_NoMipmaps;
    }
    FTextureFormatSettings FormatSettings;
    RasterTexture->SetLayerFormatSettings(0, FormatSettings);

    RasterTexture->UpdateResource();

    MetaData.RasterTexture = RasterTexture;
    UDataTable* DataTable = LoadObject<UDataTable>(Package, *DataTableAssetName);
    if (!DataTable) {
      DataTable = NewObject<UDataTable>(Package, *DataTableAssetName, RF_Public | RF_Standalone);
      DataTable->RowStruct = FRasterDataRow::StaticStruct();
    }
    DataTable->AddRow(FName(DatasetName), MetaData);
    Package->SetDirtyFlag(true);

    UPackage::SavePackage(Package, NULL, RF_Public | RF_Standalone,
      *FPackageName::LongPackageNameToFilename(PackageName, *FPackageName::GetAssetPackageExtension()),
      GLog, nullptr, false, true, SAVE_NoError);
  }
  else {
    UE_LOG(LogTemp, Warning, TEXT("Could not create package to save!"));
  }
}
#endif

void URasterDataset::CreateTransientTexture()
{
  //Read image data:
  FImageData ImageData;
  ImageData.ReadImageData(ImageFilePath);

  RasterTexture = UTexture2D::CreateTransient(ImageData.Width, ImageData.Height, ImageData.PixelFormat);
  auto Mip = &RasterTexture->PlatformData->Mips[0];
  void* TextureData = Mip->BulkData.Lock(LOCK_READ_WRITE);
  FMemory::Memcpy(TextureData, ImageData.RawPixelData.GetData(), ImageData.RawPixelData.Num());
  Mip->BulkData.Unlock();
  RasterTexture->UpdateResource();
}
