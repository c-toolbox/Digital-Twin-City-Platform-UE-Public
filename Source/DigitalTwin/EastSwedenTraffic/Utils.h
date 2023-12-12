#pragma once
#include "../DatasetUtils/ScenarioMetadata.h"
#include "Utils.generated.h"

USTRUCT(BlueprintType)
struct DIGITALTWIN_API FTrafficData
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(BlueprintReadWrite)
	FString Id;
	UPROPERTY(BlueprintReadWrite)
	FString Type;
	UPROPERTY(BlueprintReadWrite)
	int32 TimeStamp;
	UPROPERTY(BlueprintReadWrite)
	FVector Position;
	UPROPERTY(BlueprintReadWrite)
	float Heading;
	UPROPERTY(BlueprintReadWrite)
	FColor Color;
	UPROPERTY(BlueprintReadWrite)
	float Radius;
	UPROPERTY(BlueprintReadWrite)
	FString Status;
};

USTRUCT(BlueprintType)
struct DIGITALTWIN_API FMapLight
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Utils)
	FString Id;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Utils)
	FVector2D Pos;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Utils)
	FLinearColor Color;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Utils)
	FString TypeOfMessage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Utils)
	bool Enabled;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Utils)
	float Height;
};

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
struct DIGITALTWIN_API FActivateMap
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Utils)
	FString Datasets;
};

USTRUCT(BlueprintType)
struct DIGITALTWIN_API FResetCall
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Utils)
	FString Misc;
};

UENUM(BlueprintType)
enum class ERequestType : uint8
{
	Scenario,
	Light,
};

/**
 *
 *
 *
 *
 *
 *
 */

USTRUCT(BlueprintType)
struct DIGITALTWIN_API FRequest
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(BlueprintReadWrite)
	FString Type;
};

/**
 *
 *
 *
 *
 *
 *
 */

USTRUCT(BlueprintType)
struct DIGITALTWIN_API FMapLightRequest : public FRequest
{
	GENERATED_USTRUCT_BODY()

	FMapLightRequest() { Type = "MapLightRequest"; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Utils)
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Utils)
	float Northing = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Utils)
	float Easting = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Utils)
	float Height = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Utils)
	FString Color = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Utils)
	FString TypeOfMessage = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Utils)
	bool Enabled = true;
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
struct DIGITALTWIN_API FActiveDatasetRequest : public FRequest
{
	GENERATED_USTRUCT_BODY()
	FActiveDatasetRequest() { Type = "ActiveDatasetRequest"; }
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Utils)
	FString Datasets;
};

USTRUCT(BlueprintType)
struct DIGITALTWIN_API FDeactiveDatasetRequest : public FRequest
{
	GENERATED_USTRUCT_BODY()
	FDeactiveDatasetRequest() { Type = "DeactivateDatasetRequest"; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Utils)
	FString Datasets;
};

USTRUCT(BlueprintType)
struct DIGITALTWIN_API FRestApplication : public FRequest
{
	GENERATED_USTRUCT_BODY()
	FRestApplication() { Type = "Reset"; }
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Utils)
	FString Misc;
};

/**
 *
 *
 *
 *
 *
 *
 */

USTRUCT(BlueprintType)
struct DIGITALTWIN_API FResponse
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(BlueprintReadWrite)
	FString Type;
};

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

USTRUCT()
struct DIGITALTWIN_API FServerSendAllScenariosResponse : public FResponse
{
	GENERATED_USTRUCT_BODY()
	FServerSendAllScenariosResponse() { Type = "ScenarioResponse"; }
	UPROPERTY()
	TArray<FScenario> Scenarios;
};
