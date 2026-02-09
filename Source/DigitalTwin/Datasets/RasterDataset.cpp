// Fill out your copyright notice in the Description page of Project Settings.

#include "RasterDataset.h"
#include "GenericPlatform/GenericPlatformFile.h"

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