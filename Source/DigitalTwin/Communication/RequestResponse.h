#pragma once
#include "RequestResponse.generated.h"

USTRUCT(BlueprintType)
struct DIGITALTWIN_API FSkyLight2
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Utils)
	uint8 Day = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Utils)
	uint8 Month = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Utils)
	uint8 Year = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Utils)
	float SolarTime = 0.0f;
};

USTRUCT(BlueprintType)
struct DIGITALTWIN_API FRequest
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(BlueprintReadWrite)
	FString Type;
};

USTRUCT(BlueprintType)
struct DIGITALTWIN_API FResponse
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(BlueprintReadWrite)
	FString Type;
};

USTRUCT(BlueprintType)
struct DIGITALTWIN_API FSetMapMarkerRequest : public FRequest
{
	GENERATED_USTRUCT_BODY()

	FSetMapMarkerRequest() { Type = "SetMapMarkerRequest"; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Utils)
	FString ID; // A unique id

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Utils)
	float U = -1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Utils)
	float V = -1.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Utils)
	float Radius = -1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Utils)
	FString Color = "#FFFFFF"; //Hex

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Utils)
	float Opacity = -1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Utils)
	float Density = -1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Utils)
	float Emission = -1.0f;
	
};

USTRUCT(BlueprintType)
struct DIGITALTWIN_API FRemoveMapMarkerRequest : public FRequest
{
	GENERATED_USTRUCT_BODY()
	FRemoveMapMarkerRequest() { Type = "RemoveMapMarkerRequest"; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Utils)
	FString ID;
};

USTRUCT(BlueprintType)
struct DIGITALTWIN_API FMapMarkerResponse : public FResponse
{
	GENERATED_USTRUCT_BODY()
	FMapMarkerResponse() { Type = "MapMarkerResponse"; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Utils)
	TArray<FString> IDs;
};

USTRUCT(BlueprintType)
struct DIGITALTWIN_API FLightRequest : public FRequest
{
	GENERATED_USTRUCT_BODY()

	FLightRequest() : Day(0), Year(0), Solar_Time(0.0f) { Type = "LightRequest"; }
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Utils)
	uint8 Day = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Utils)
	uint8 Month = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Utils)
	uint8 Year = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Utils)
	float Solar_Time = 0.0f;
};

USTRUCT(BlueprintType)
struct DIGITALTWIN_API FNDI
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Utils)
	FString Machine;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Utils)
	FString Stream;
};

USTRUCT(BlueprintType)
struct DIGITALTWIN_API FSliceCrop
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Utils)
	float Min_U = -1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Utils)
	float Max_U = -1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Utils)
	float Min_V = -1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Utils)
	float Max_V = -1.0f;
};

USTRUCT(BlueprintType)
struct DIGITALTWIN_API FCircleCrop
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Utils)
	float U = -1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Utils)
	float V = -1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Utils)
	float Radius = -1.0f;
};

USTRUCT(BlueprintType)
struct DIGITALTWIN_API FCrop
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Utils)
	FString Type = "None";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Utils)
	FSliceCrop Slice;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Utils)
	FCircleCrop Circle;
};

USTRUCT(BlueprintType)
struct DIGITALTWIN_API FFlow
{

	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Utils)
	FString Texture; // Folder/File

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Utils)
	float Scale = -1.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Utils)
	float Speed = -1.0f;
};

USTRUCT(BlueprintType)
struct DIGITALTWIN_API FMovie
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Utils)
	float Speed = -1.0; // Folder/File
};

USTRUCT(BlueprintType)
struct DIGITALTWIN_API FLayerDef
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Utils)
	FString Type; // image|movie|flow|color|base64|ndi
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Utils)
	FString Name; // Folder/File

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Utils)
	float Opacity = -1.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Utils)
	float Emission = -1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Utils)
	FCrop Crop;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Utils)
	FFlow Flow; 
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Utils)
	FMovie Movie;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Utils)
	FString Color; //Hexf

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Utils)
	FNDI Ndi;
	
};

USTRUCT(BlueprintType)
struct DIGITALTWIN_API FLayerRequest : public FRequest
{
	GENERATED_USTRUCT_BODY()
	FLayerRequest() { Type = "LayerRequest"; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Utils)
	bool Flush = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Utils)
	TArray<FLayerDef> Layers;
};

USTRUCT(BlueprintType)
struct DIGITALTWIN_API FLayerResponse : public FResponse
{
	GENERATED_USTRUCT_BODY()
	FLayerResponse() { Type = "LayerResponse"; }
	UPROPERTY(BlueprintReadWrite)
	FString Name;
	UPROPERTY(BlueprintReadWrite)
	FString Error;
};

USTRUCT(BlueprintType)
struct DIGITALTWIN_API FRecacheRequest : public FRequest
{
	GENERATED_USTRUCT_BODY()
	FRecacheRequest() { Type = "RecacheRequest"; }
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Utils)
	FString Path = "Datasets"; //The path to recursively recache
};

USTRUCT(BlueprintType)
struct DIGITALTWIN_API FRecacheResponse : public FResponse
{
	GENERATED_USTRUCT_BODY()
	FRecacheResponse() { Type = "RecacheResponse"; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Utils)
	int Index = 0; //The current index that was updated

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Utils)
	int Max = 0; //The total amount of files that are being recached
};

USTRUCT(BlueprintType)
struct DIGITALTWIN_API FResetRequest : public FRequest
{
	GENERATED_USTRUCT_BODY()
	FResetRequest() { Type = "Reset"; }
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Utils)
	FString Misc;
};
USTRUCT()
struct DIGITALTWIN_API FResetResponse : public FResponse
{
	GENERATED_USTRUCT_BODY()
	FResetResponse() { Type = "ResetResponse"; }
};

/*
 * One way or multidirectional use
 */

USTRUCT(BlueprintType)
struct DIGITALTWIN_API FErrorResponse : public FResponse
{
	GENERATED_USTRUCT_BODY()
	FErrorResponse() { Type = "ErrorResponse"; }
	FString Error;
};

//PingResponse is also used for PingRequest as they have the same format
USTRUCT(BlueprintType)
struct DIGITALTWIN_API FPingResponse : public FResponse
{
	GENERATED_USTRUCT_BODY()
	FPingResponse() { Type = "PingResponse"; }
};

USTRUCT(BlueprintType)
struct DIGITALTWIN_API FLightResponse : public FResponse
{
	GENERATED_USTRUCT_BODY()
	FLightResponse() { Type = "LightResponse"; }
	UPROPERTY(BlueprintReadWrite)
	FString TimeOfDay;
};