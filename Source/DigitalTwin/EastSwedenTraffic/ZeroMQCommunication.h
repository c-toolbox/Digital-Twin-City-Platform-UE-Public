#pragma once

#include "CoreMinimal.h"
#include "HAL/Runnable.h"
#include "Subsystems/WorldSubsystem.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Subsystems/SubsystemCollection.h"
#include "ZeroMqWorker.h"
#include "ZeroMQCommunication.generated.h"


UCLASS()
class DIGITALTWIN_API UZeroMqCommunication :  public UTickableWorldSubsystem {
	GENERATED_BODY()
	DECLARE_MULTICAST_DELEGATE_OneParam(FTrafficUpdate2,const FTrafficData &);
public:
	
	FTrafficUpdate2 OnTrafficUpdate2;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
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
	bool SaveStuff(UObject* ObjectToSave);

#if WITH_EDITOR
	void LoadTexture(FString &Path);
	void RemoveTexture(FString &Path);	
#endif
	
private:
	
	TMap<FString,FZeroMqWorker*> Workers;
	TSharedPtr<TQueue<FTrafficData>> Worker_Queue;

};
