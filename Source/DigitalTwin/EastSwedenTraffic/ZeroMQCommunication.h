#pragma once

#include "BlocksWorker.h"
#include "CoreMinimal.h"
#include "HAL/Runnable.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Subsystems/SubsystemCollection.h"
#include "Subsystems/WorldSubsystem.h"
#include "Utils.h"
#include "WebSocketWorker.h"
#include "ZeroMqWorker.h"
#include "ZeroMQCommunication.generated.h"

// #TODO Move websocket stuff to other Subsystem  

UCLASS()
class DIGITALTWIN_API UZeroMqCommunication : public UTickableWorldSubsystem {
  GENERATED_BODY()
  //
  DECLARE_MULTICAST_DELEGATE_OneParam(FTrafficUpdate ,const FTrafficData &);
  //
  DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMapLightUpdate ,const FMapLight& , Data);
  DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLightUpdate    ,const FSkyLight2& , Data);
  DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FReset, const FString& , Misc);
  //
  DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDatasetUpdate  ,const FActivateMap& , Dataset, bool , Enable);
  //
public:
  FTrafficUpdate OnTrafficUpdate;

  UPROPERTY(BlueprintAssignable)
  FDatasetUpdate OnDatasetUpdate;

  UPROPERTY(BlueprintAssignable)
  FMapLightUpdate OnMapLightUpdate;

  UPROPERTY(BlueprintAssignable)
  FLightUpdate OnLightUpdate;

  UPROPERTY(BlueprintAssignable)
  FReset OnReset;
  
  virtual void Initialize(FSubsystemCollectionBase &Collection) override;
  virtual void Deinitialize() override;
  virtual void Tick(float DeltaTime) override;
  virtual TStatId GetStatId() const override;
  
  UFUNCTION(BlueprintCallable)
  void StartWorker(FString WorkerName, FString Interface);

  UFUNCTION(BlueprintCallable)
  void StartWebSocketWorker(FString WorkerName, FString Interface);

  UFUNCTION(BlueprintCallable)
  void StopWorker(const FString &WorkerName) const;

  UFUNCTION(BlueprintCallable)
  void StopWorkers();

  UFUNCTION(BlueprintCallable)
  void SendRequest(const FString WorkerName);

  UFUNCTION(BlueprintCallable)
  void SendWebSocketRequest(const FString WorkerName,FRequest Request);

  UFUNCTION(BlueprintCallable)
  void SendWebSocketResponse(const FString WorkerName,FResponse Response);

private:
  // TODO Refactor this .... !
  TMap<FString, FZeroMqWorker *>   Workers;
  TMap<FString, FBlocksWorker *>   TempWorkers;
  TMap<FString, FWebSocketWorker *>   WebWorkers;
  TSharedPtr<TQueue<FTrafficData>> Worker_Queue;
  TSharedPtr<TQueue<FRequest*>> Message_Queue;
};
