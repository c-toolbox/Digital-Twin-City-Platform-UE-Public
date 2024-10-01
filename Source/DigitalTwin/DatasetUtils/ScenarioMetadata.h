#pragma once
#include "CoreMinimal.h"
#include "Engine/DataTable.h"

#include "ScenarioMetadata.generated.h"

USTRUCT(BlueprintType, Category = ScenarioMetaData)
struct FDatasetWrapper
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ScenarioMetaData)
		FString DatasetName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ScenarioMetaData)
		FString CatalogName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ScenarioMetaData)
		int32 ZOrder;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ScenarioMetaData)
		float Opacity = 0.0f;
};

USTRUCT(BlueprintType, Category = ScenarioMetaData)
struct FAnnotationInfo
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ScenarioMetaData)
		FString Annotation1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ScenarioMetaData)
		FString Annotation2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ScenarioMetaData)
		FString ImagePath;
};

USTRUCT(BlueprintType, Category = ScenarioMetaData)
struct FScenarioMetadata
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ScenarioMetaData)
	  FString ScenarioName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ScenarioMetaData)
		FAnnotationInfo Info;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ScenarioMetaData)
	  TArray<FDatasetWrapper> CatalogDatasets;
};

/**
 *
 *
 *
 */
UENUM(BlueprintType, Category = Scenario)
enum class ScenarioSubSectionType : uint8 {
	Layer UMETA(DisplayName = "Layer"),
	PointLayer UMETA(DisplayName = "PointLayer"),
};

USTRUCT(BlueprintType, Category = Test)
struct DIGITALTWIN_API FScenarioSubSection {
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Scenario)
	FString Title = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Scenario)
	FString Comment = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Scenario)
	FString Text1 = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Scenario)
	FString Text2 = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Scenario)
	FString Legend1 = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Scenario)
	FString Legend2 = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Scenario)
	FString Filenames = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Scenario)
	bool Default = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Scenario)
	bool IsIdle = false;
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Scenario)
	TArray<FString> FilenameArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Scenario)
	ScenarioSubSectionType SectionType;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Scenario)
	FVector2D Lat_Lon = FVector2D::Zero();

};

USTRUCT(BlueprintType, Category = Test)
struct DIGITALTWIN_API FScenarioSection {
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Scenario)
	TArray<FScenarioSubSection> SectionObject;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Scenario)
	FString Label1;

	//Label1
	TMap<FString,TArray<FScenarioSubSection>> SectionContent; 
	
};

USTRUCT(BlueprintType, Category = Test)
struct DIGITALTWIN_API FScenario {
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Scenario)
	TArray<FScenarioSection> Sections;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Scenario)
	FString Title;
};
