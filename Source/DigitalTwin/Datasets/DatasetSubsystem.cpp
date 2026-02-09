// Fill out your copyright notice in the Description page of Project Settings.


#include "DigitalTwin/Datasets/DatasetSubsystem.h"
#include "DigitalTwin/Datasets/RasterDataset.h"

void UDatasetSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UDatasetSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

UTexture2D* UDatasetSubsystem::GetDatasetAsTexture(const FString &FilePath) {
	//We check if the file exist. Might be redundant as look for valid files in the Blueprint that calls this function
	if(FPaths::FileExists(FilePath)) {
		UE_LOG(LogTemp, Warning, TEXT("File %s ,exists i Filedatabase !"), *FilePath);
	} else {
		UE_LOG(LogTemp, Warning, TEXT("File dosen`t exists !"));
		return nullptr;
	}
  
	//Read image and create texture  
	FImageData ImageData;
	ImageData.ReadImageData(FilePath);
	const auto RasterTexture = UTexture2D::CreateTransient(ImageData.Width, ImageData.Height, ImageData.PixelFormat);
	const auto Mip = &RasterTexture->GetPlatformData()->Mips[0];
	void* TextureData = RasterTexture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
	FMemory::Memcpy(TextureData, ImageData.RawPixelData.GetData(), ImageData.RawPixelData.Num());

	//Change some settings and update the texture resource
	RasterTexture->GetPlatformData()->Mips[0].BulkData.Unlock();
	//TODO: Check what is the default Compression Setting, and if we need to change it for better image quality
	RasterTexture->UpdateResource();

	if (RasterTexture->IsValidLowLevel()) {
		return RasterTexture;
	} 

	return nullptr;
}

//Changes settings on texture such that the texture works as a flowmap input
void UDatasetSubsystem::SetFlowmapSettings(UTexture2D* Texture)
{
	Texture->CompressionSettings = TC_VectorDisplacementmap;
	Texture->SRGB = false;
	Texture->UpdateResource();
}
