#pragma once

#include "BlocksWorker.h"
#include "CoreMinimal.h"
#include "HAL/Runnable.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Subsystems/SubsystemCollection.h"
#include "Subsystems/WorldSubsystem.h"
#include "Utils.h"
#include "VectorTypes.h"
#include "WebSocketWorker.h"
#include "ZeroMqWorker.h"
#include "ZeroMQCommunication.generated.h"


// #TODO Move websocket stuff to other Subsystem  

UCLASS()
class DIGITALTWIN_API UZeroMqCommunication : public UTickableWorldSubsystem {
  GENERATED_BODY()
  /*
   * 
   */
  DECLARE_MULTICAST_DELEGATE_OneParam(FTrafficUpdate ,const FTrafficData &);
  /*
   * 
   */
  DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGenericResponseDelegate ,const FString& ,ResponseString);

  DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTrafficUpdateBlue , FVector , Position);
 
public:
  /*
   *
   */
  FTrafficUpdate OnTrafficUpdate;

  UPROPERTY(BlueprintAssignable)
  FGenericResponseDelegate OnGenericResponse;

  UPROPERTY(BlueprintAssignable)
  FTrafficUpdateBlue OnTrafficUpdateBlue;

  
  virtual void Initialize(FSubsystemCollectionBase &Collection) override;
  virtual void Deinitialize() override;
  virtual void Tick(float DeltaTime) override;
  virtual TStatId GetStatId() const override;
  
  UFUNCTION(BlueprintCallable)
  void StartWorker(FString WorkerName, FString Interface);
    
  UFUNCTION(BlueprintCallable)
  void StopWorkers();

  UFUNCTION(BlueprintCallable)
  void SendRequest(const FString WorkerName);

private:
  // TODO Refactor this .... !
  TMap<FString, FZeroMqWorker *>   Workers;
  TMap<FString, FBlocksWorker *>   TempWorkers;
  TMap<FString, FWebSocketWorker *>   WebWorkers;
  TSharedPtr<TQueue<FTrafficData>> Worker_Queue;
  TSharedPtr<TQueue<FRequest*>> Message_Queue;
};
