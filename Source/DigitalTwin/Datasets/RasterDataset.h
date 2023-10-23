
#pragma once
#include "Metadata.h"

#include "Dataset.h"
#include "CoreMinimal.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"

#include "RasterDataset.generated.h"

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

/**
* URasterDataset - Holds RasterDataset data
*
* This is not the scene representation of a Raster
* TODO?:
* - "real" rasters could be skewed, but lets not allow that for now...
*/

UCLASS()
class DIGITALTWIN_API URasterDataset : public UDataset
{
  GENERATED_BODY()
public:
  virtual void ActivateDataset(UWorld* World) override;
  virtual void ToggleDataset(UWorld* World) override;
  virtual void DisableDataset(UWorld* World) override;

  void SetMetaData(FRasterDataRow InMetaData)
  {
    MetaData = InMetaData;
    if (InMetaData.RasterTexture) {
      RasterTexture = InMetaData.RasterTexture;
    }
    SetDatasetName(MetaData.Name);
  }

  void SetTexture(UTexture2D* InTexture)
  {
    RasterTexture = InTexture;
  }

  void SetImageFilePath(FString InImageFilePath)
  {
    ImageFilePath = InImageFilePath;
  }

  const FRasterDataRow& GetMetaData() const
  {
    return MetaData;
  }

  //Gets the currently held RasterTexture. 
  //Might return nullptr if no Texture is being held
  UFUNCTION()
    UTexture2D* GetRasterTexture() const;

  /*
  * Attempt to load a cached texture from the given Package.
  * This requires the CatalogName/DatasetName to be set.
  *
  * Returns true if a Texture and MetaData was found in the Package
  * Returns false otherwise.
  */
  bool TryLoadFromCache(UPackage* Package, const FString& PackageName);

  /*
  * Loads in RasterTexture from the ImageFilePath.
  * If running in-editor, this will also cache the Texture in the
  * given Package for future use.
  *
  * Otherwise this will create a Transient(runtime) texture which can be used
  * for the current gameplay session.
  */
  void LoadTextureFromImageFile(UPackage* Package, const FString& PackageName);

private:
#if WITH_EDITOR
  /*
  * Generates a Texture from the ImageFile and puts it into the Cache.
  * Also searches for a metadata file and caches it too.
  */
  void GenerateTextureCache(UPackage* Package, const FString& PackageName);
#endif
  /*
  * Creates a Transient texture, which only lives for the current gameplay session
  * This is called in non-editor builds.
  */
  void CreateTransientTexture();

  UPROPERTY()
    UTexture2D* RasterTexture;
  UPROPERTY()
    class ARasterActor* RasterActor;

  FRasterDataRow MetaData;
  FString ImageFilePath;
};

