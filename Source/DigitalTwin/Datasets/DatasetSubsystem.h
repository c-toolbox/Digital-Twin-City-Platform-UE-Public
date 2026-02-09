// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "DatasetSubsystem.generated.h"

UCLASS()
class DIGITALTWIN_API UDatasetSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
  
public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable, Category = "DigitalTwin|Datasets")
	UTexture2D* GetDatasetAsTexture(const FString& FilePath);

	UFUNCTION(BlueprintCallable, Category = "DigitalTwin|Datasets")
	void SetFlowmapSettings(UTexture2D* Texture);
};
