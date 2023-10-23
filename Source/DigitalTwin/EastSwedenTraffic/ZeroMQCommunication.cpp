#include "ZeroMQCommunication.h"

#include "EngineUtils.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"


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

void UZeroMqCommunication::Initialize(FSubsystemCollectionBase& Collection) {
	Super::Initialize(Collection);
}

//TODO : Review this one more time !!
void UZeroMqCommunication::Deinitialize() {

	TArray<FString> Keys; 
	Workers.GetKeys(Keys);

	for(const auto & Key : Keys) {
		const auto Worker = Workers[Key];
		if(Worker) {
			Worker->Stop();
			FPlatformProcess::Sleep(0.5f);
			Worker->Exit();
			FPlatformProcess::Sleep(0.5f);
			delete Worker;	
		}
	}
}

//TODO : Note this maybe a bit of a problem
void UZeroMqCommunication::Tick(const float DeltaTime) {
	Super::Tick(DeltaTime);
	if(Worker_Queue.IsValid()) {
		TArray<FTrafficData> Data_To_Send;
		while(!Worker_Queue->IsEmpty()) {
			FTrafficData Temp;
			Worker_Queue->Dequeue(Temp);
			if(Temp.Color == FColor::Red) {
				UE_LOG(LogTemp, Warning, TEXT("Connected to tcp://localhost:5557 !!"));
			}
			OnTrafficUpdate2.Broadcast(Temp);
		}
	}
}

TStatId UZeroMqCommunication::GetStatId() const {
	RETURN_QUICK_DECLARE_CYCLE_STAT(UZeroMQCommunication, STATGROUP_Tickables);
}

void UZeroMqCommunication::StartWorker() {
	// Load the asset registry module
	/*
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::Get().LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));


	TArray<FString> SelectedPaths;
	SelectedPaths.Add("/Game/Datasets/");
	SelectedPaths.Add("/Game/Textures/");

	FString PackageName = TEXT("/Game/est/");
	//PackageName += FString::SanitizeFloat(GetWorld()->GetTimeSeconds());

	UPackage* Package = CreatePackage(*PackageName);
	*/
	//Package->FullyLoad();
	//Package->MarkPackageDirty();



	//FString PackageFileName = FPackageName::LongPackageNameToFilename(PackageName, FPackageName::GetAssetPackageExtension());
	//bool bSaved = UPackage::SavePackage(Package, nullptr, EObjectFlags::RF_Public | EObjectFlags::RF_Standalone, *PackageFileName, GError, nullptr, true, true, SAVE_NoError);
	// /Game/Datasets/Nkpg/Byggnads_ar		
	//FAssetRegistryModule& AssetRegistryModule = FModuleManager::Get().LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
	// Form a filter from the paths
	//TArray<FString> SelectedPaths;
	
	//SelectedPaths.Add("/Content/Test/Nkpg");
	//SelectedPaths.Add("/Game/Textures/");
	/*
	FARFilter Filter;
	Filter.bRecursivePaths = true;
	for (int32 PathIdx = 0; PathIdx < SelectedPaths.Num(); ++PathIdx)
	{
		const FString& Path = SelectedPaths[PathIdx];
		new (Filter.PackagePaths) FName(*Path);
	}
	UE_LOG(LogTemp, Warning, TEXT("Llllllooooooogggggg "));

	// Query for a list of assets in the selected paths
	TArray<FAssetData> AssetList2;
	AssetRegistryModule.Get().GetAllAssets( AssetList2,false);
	
	// Query for a list of assets in the selected paths
	TArray<FAssetData> AssetList;
	AssetRegistryModule.Get().GetAssets(Filter, AssetList);

	// Form a list of unique package names from the assets
	TSet<FName> UniquePackageNames;
	for (int32 AssetIdx = 0; AssetIdx < AssetList.Num(); ++AssetIdx)
	{
		UniquePackageNames.Add(AssetList[AssetIdx].PackageName);
	}
	UE_LOG(LogTemp, Warning, TEXT("Llllllooooooogggggg "));
	// Add all unique package names to the output
	for ( auto PackageIt = UniquePackageNames.CreateConstIterator(); PackageIt; ++PackageIt )
	{
		UE_LOG(LogTemp, Warning, TEXT("Llllllooooooogggggg :  %s"), *(*PackageIt).ToString() );
	}
	*/
	
	FString Path = FPaths::ProjectDir()+"/Datasets/Test/1_2_3.png";
#if WITH_EDITOR
	//LoadTexture(Path);
	//RemoveTexture(Path);
#endif 
}

void UZeroMqCommunication::StartWorker2(const FString WorkerName,const FString IP_AdressString) {
	
	if(!Worker_Queue.IsValid()) {
		Worker_Queue = MakeShared<TQueue<FTrafficData>>();
	}

	const auto w = new FZeroMqWorker(Worker_Queue,IP_AdressString,WorkerName);
	w->Init();
	Workers.Add(WorkerName,w);
}

void UZeroMqCommunication::StopWorker(const FString &WorkerName) const  {
	
	const auto Worker = Workers[WorkerName];
	if(Worker) {
		Worker->Stop();
	}
}

void UZeroMqCommunication::StopWorkers() {
	TArray<FString> Keys; 
	Workers.GetKeys(Keys);

	for(const auto & Key : Keys) {
		const auto Worker = Workers[Key];
		if(Worker) {
			Worker->Stop();
			FPlatformProcess::Sleep(0.5f);
		}
	}
}

#if WITH_EDITOR

void UZeroMqCommunication::LoadTexture(FString &ImageFilePath) {
	/*
	FString PackageRoot = TEXT("/Game/Test");
	//PackageRoot += FString::SanitizeFloat(GetWorld()->GetTimeSeconds());
	// #########################################################################################
	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	TArray64<uint8> CompressedImageData;
	if (!FFileHelper::LoadFileToArray(CompressedImageData, *ImageFilePath)) {
		UE_LOG(LogTemp, Warning, TEXT("Could not load file: %s"), *ImageFilePath);
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("Could load file: %s"), *ImageFilePath);

	const uint8* Buffer = CompressedImageData.GetData();
	const uint64 BufLen = CompressedImageData.Num();
	TSharedPtr<IImageWrapper> PngImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);

	FImageData ImageData;
	
	if (PngImageWrapper.IsValid() && PngImageWrapper->SetCompressed(Buffer, BufLen))
	{
		//TODO: Ensure we are picking valid image formats
		ImageData.TextureFormat = TSF_Invalid;
		ImageData.BitDepth = PngImageWrapper->GetBitDepth();
		ImageData.Format = PngImageWrapper->GetFormat();
		ImageData.PixelFormat = EPixelFormat::PF_Unknown;
		if (ImageData.Format == ERGBFormat::Gray)
		{
			if (ImageData.BitDepth <= 8) {
				ImageData.TextureFormat = TSF_G8;
				ImageData.PixelFormat = PF_G8;
				ImageData.Format = ERGBFormat::Gray;
				ImageData.BitDepth = 8;
			}
			else if (ImageData.BitDepth == 16) {
				ImageData.TextureFormat = TSF_G16;
				ImageData.PixelFormat = PF_G16;
				ImageData.Format = ERGBFormat::Gray;
				ImageData.BitDepth = 16;
			}
		}
		else if (ImageData.Format == ERGBFormat::RGBA || ImageData.Format == ERGBFormat::BGRA) {
			if (ImageData.BitDepth <= 8) {
				ImageData.TextureFormat = TSF_BGRA8;
				ImageData.Format = ERGBFormat::BGRA;
				ImageData.PixelFormat = PF_B8G8R8A8;
				ImageData.BitDepth = 8;
			}
			else if (ImageData.BitDepth == 16) {
				ImageData.TextureFormat = TSF_RGBA16;
				ImageData.Format = ERGBFormat::RGBA;
				ImageData.PixelFormat = PF_R16G16B16A16_UINT;
				ImageData.BitDepth = 16;
			}
		}

		if (ImageData.TextureFormat == TSF_Invalid) {
			UE_LOG(LogTemp, Error, TEXT("PNG file contains data in an unsupported format."));
		}

		if (PngImageWrapper->GetRaw(ImageData.Format, ImageData.BitDepth, ImageData.RawPixelData)) {
			ImageData.Height = PngImageWrapper->GetHeight();
			ImageData.Width = PngImageWrapper->GetWidth();
		}
	} else {
		UE_LOG(LogTemp, Error, TEXT("PNG file contains unexpected data!"));
	}
	// #########################################################################################
	UPackage* Package = CreatePackage(*PackageRoot);
	auto test = Package->GetMetaData();

	FString TextureAssetName = "sldkjfsdlk";
	auto RasterTexture = NewObject<UTexture2D>(Package, *TextureAssetName, RF_Public | RF_Standalone);
	if (RasterTexture != NULL)
	{
		RasterTexture->Source.Init(ImageData.Width, ImageData.Height, 1, 1, ImageData.TextureFormat, ImageData.RawPixelData.GetData());
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

		Package->SetDirtyFlag(true);

		UPackage::SavePackage(Package, NULL, RF_Public | RF_Standalone,
		  *FPackageName::LongPackageNameToFilename(PackageRoot, *FPackageName::GetAssetPackageExtension()),
		  GLog, nullptr, false, true, SAVE_NoError);
	}
	*/
	return;
}	


void UZeroMqCommunication::RemoveTexture(FString &ImageAssestName)
{
	/*
	FString PackageRoot = TEXT("/Game/Test");
	TArray<UObject*> TextureAssets;
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

	TArray<FAssetData> Assets;
	AssetRegistry.GetAssetsByPath(FName(*PackageRoot), Assets, true);
	
 	for (auto Object : Assets)
	{
		UE_LOG(LogTemp, Error, TEXT("Texture Asset %s"),*Object.GetFullName());
 		AssetRegistry.AssetDeleted(Object.GetAsset());
 		GEngine->ForceGarbageCollection(true);
	}
	*/
}
#endif

bool UZeroMqCommunication::SaveStuff(UObject* ObjectToSave)
{
	
		UPackage* Package = ObjectToSave->GetPackage();
		const FString PackageName = Package->GetName();
		const FString PackageFileName = FPackageName::LongPackageNameToFilename(PackageName, FPackageName::GetAssetPackageExtension());

		FSavePackageArgs SaveArgs;
	
		// This is specified just for example
		{
			SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
			SaveArgs.SaveFlags = SAVE_NoError;
		}
	
		

		const bool bSucceeded = UPackage::SavePackage(Package, nullptr, RF_Public | RF_Standalone,*PackageFileName,GLog,nullptr, false, true, SAVE_NoError);

		if (!bSucceeded) {
			UE_LOG(LogTemp, Error, TEXT("Package '%s' wasn't saved!"), *PackageName)
			return false;
		}

		UE_LOG(LogTemp, Warning, TEXT("Package '%s' was successfully saved"), *PackageName)
		return true;
}