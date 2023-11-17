#pragma once
#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "../DatasetUtils/ScenarioMetadata.h"
#include "ScenarioSubsystem.generated.h"

UENUM(BlueprintType, Category = ScenarioSubsystem)
enum class Language : uint8 {
	Swedish,
	English
};

UCLASS()
class DIGITALTWIN_API UScenarioSubsystem : public UWorldSubsystem {
	GENERATED_BODY()
public:
	void Initialize(FSubsystemCollectionBase& Collection) override;
	void Deinitialize() override;
	
	UFUNCTION(BlueprintCallable, Category = "DigitalTwin|ScenarioDatasets")
		TArray<FScenario> GetScenarios(Language lang);
	
	UFUNCTION(BlueprintCallable, Category = "DigitalTwin|ScenarioDatasets")
		TArray<FString> GetSectionsForScenarios(FScenarioSection& Section);

	UFUNCTION(BlueprintCallable, Category = "DigitalTwin|ScenarioDatasets")
		TArray<FString> GetSubSectionsForScenarios(FScenarioSection &Subsections);

	UFUNCTION(BlueprintCallable, Category = "DigitalTwin|ScenarioDatasets")
		void SetupScenarios();

	UFUNCTION(BlueprintCallable, Category = "DigitalTwin|ScenarioDatasets")
		void Reload();

private:	
	void ParseScenarios(FString DatasetRootDir, TArray<FString> ScenarioFiles, Language type);
	TArray<FScenario> SV_Scenarios_;
	TArray<FScenario> EN_Scenarios_;
};
