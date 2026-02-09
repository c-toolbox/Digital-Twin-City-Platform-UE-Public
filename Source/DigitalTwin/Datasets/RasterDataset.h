
#pragma once
#include "CoreMinimal.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"

//#include "RasterDataset.generated.h"

/*
* The FImageData is an intermediate struct for keeping raw image data
* This is later translated to a UTexture2D texture.
*/
struct FImageData
{
  FImageData()
  {}
  /*
  * Attempt to read image data from an ImageFilePath.
  * This will fill/initialize the struct and attempt a blocking read from drive.
  */
  void ReadImageData(const FString& ImageFilePath);

  ETextureSourceFormat TextureFormat = TSF_Invalid;
  ERGBFormat Format = ERGBFormat::Invalid;
  EPixelFormat PixelFormat = EPixelFormat::PF_Unknown;
  int32 Width = 0;
  int32 Height = 0;
  int32 BitDepth = 0;
  TArray64<uint8> RawPixelData;
private:
  /*
  * Parse a PNG image. This is internally used by the ReadImageData function
  */
  void ParsePNG(IImageWrapperModule& ImageWrapperModule, const TArray64<uint8>& CompressedImageData);
};