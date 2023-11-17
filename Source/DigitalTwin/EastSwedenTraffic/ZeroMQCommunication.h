#pragma once

#include "CoreMinimal.h"
#include "HAL/Runnable.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Subsystems/SubsystemCollection.h"
#include "Subsystems/WorldSubsystem.h"
#include "ZeroMqWorker.h"
#include "BlocksWorker.h"
#include "ZeroMQCommunication.generated.h"

UCLASS()
class DIGITALTWIN_API UZeroMqCommunication : public UTickableWorldSubsystem {
  GENERATED_BODY()
  DECLARE_MULTICAST_DELEGATE_OneParam(FTrafficUpdate2, const FTrafficData &);

public:
  FTrafficUpdate2 OnTrafficUpdate2;
  virtual void Initialize(FSubsystemCollectionBase &Collection) override;
  virtual void Deinitialize() override;
  virtual void Tick(float DeltaTime) override;
  virtual TStatId GetStatId() const override;

  UFUNCTION(BlueprintCallable)
  void StartWorker();

  UFUNCTION(BlueprintCallable)
  void StartWorker2(FString WorkerName, FString Interface);

  UFUNCTION(BlueprintCallable)
  void StopWorker(const FString &WorkerName) const;

  UFUNCTION(BlueprintCallable)
  void StopWorkers();

  UFUNCTION(BlueprintCallable)
  void SendRequest(const FString WorkerName);

private:
  // TODO Refactor this .... !
  TMap<FString, FZeroMqWorker *>   Workers;
  TMap<FString, FBlocksWorker *>   TempWorkers;
  TSharedPtr<TQueue<FTrafficData>> Worker_Queue;
};
