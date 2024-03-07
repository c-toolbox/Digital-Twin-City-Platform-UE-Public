// --------------------------------------------------------------------------------
// Copyright 2021, Linköping University and Universeum AB
//

// --------------------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "InteractionSubsystem.generated.h"

UCLASS()
class DIGITALTWIN_API UInteractionSubsystem : public UTickableWorldSubsystem
{
  GENERATED_BODY()
public:


  // Begin USubsystem Interface
  virtual void Initialize(FSubsystemCollectionBase& Collection) override;
  virtual bool IsTickable() const override { return true; }
  virtual bool IsTickableInEditor() const override { return true; }
  virtual void Tick(float DeltaTime) override;
  virtual TStatId GetStatId() const override;
  // End USubsystemInterface


  DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeviceIdleEvent);
  DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNewInteractionEvent);

  UPROPERTY(BlueprintAssignable, Category = "Idle State")
	FOnDeviceIdleEvent OnDeviceIdle;

  UPROPERTY(BlueprintAssignable, Category = "Idle State")
	FOnNewInteractionEvent OnNewInteraction;

  UFUNCTION(BlueprintCallable)
	void SetSecondsUntilIdle(float InSecondsUntilIdle);

private:
  UFUNCTION()
	void NewInteraction(double InteractionTime);

  float SecondsUntilIdle = 300.f;
  bool bIsIdle = false;
};