#pragma once
#include "CoreMinimal.h"
#include "HAL/Runnable.h"
#include "WebSocketsModule.h" // Module definition
#include "IWebSocket.h" // Socket definition
#include "Subsystems/GameInstanceSubsystem.h"
#include "Subsystems/SubsystemCollection.h"
#include "Subsystems/WorldSubsystem.h"
#include "Utils.h"
#include "WebSocketSubsystem.generated.h"

UCLASS()
class DIGITALTWIN_API UWebSocketSubsystem : public UGameInstanceSubsystem {
  GENERATED_BODY()

public:
  virtual void Initialize(FSubsystemCollectionBase &Collection) override;
  virtual void Deinitialize() override;

  DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMapLightUpdate,
                                              const FMapLight &, Data);
  DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLightUpdate, const FSkyLight2 &,
                                              Data);
  DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FReset, const FString &, Misc);
  DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDatasetUpdate,
                                               const FActivateMap &, Dataset,
                                               bool, Enable);
  DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FActivateRealTimeTraffic, bool,
                                              OnOff);
  DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGetScenarioRequest, FString,
                                              Language);

  UPROPERTY(BlueprintAssignable, Category = "DigitalTwin|WebSocketSubsystem")
  FDatasetUpdate OnDatasetUpdate;

  UPROPERTY(BlueprintAssignable, Category = "DigitalTwin|WebSocketSubsystem")
  FMapLightUpdate OnMapLightUpdate;

  UPROPERTY(BlueprintAssignable, Category = "DigitalTwin|WebSocketSubsystem")
  FLightUpdate OnLightUpdate;

  UPROPERTY(BlueprintAssignable, Category = "DigitalTwin|WebSocketSubsystem")
  FReset OnReset;

  UPROPERTY(BlueprintAssignable, Category = "DigitalTwin|WebSocketSubsystem")
  FActivateRealTimeTraffic OnActivateRealTimeTraffic;

  UPROPERTY(BlueprintAssignable, Category = "DigitalTwin|WebSocketSubsystem")
  FGetScenarioRequest OnGetScenarioRequest;

  UFUNCTION(BlueprintCallable, Category  = "DigitalTwin|WebSocketSubsystem")
  bool Enable();

  UFUNCTION(BlueprintCallable, Category  = "DigitalTwin|WebSocketSubsystem")
  bool DelayEnable();

  UFUNCTION(BlueprintCallable, Category  = "DigitalTwin|WebSocketSubsystem")
  void SendRepsonse(FString Response);

  void HandleRequest(const FString &Message) const;
  void SendJsonResponse(const FString &Message) const;
  void SendErrorResponse(const FString &Response,const FString &Message) const;

  UFUNCTION(BlueprintCallable, Category = "DigitalTwin|WebSocketSubsystem")
  void SendScenarioJsonRepsonse(TArray<FScenario> Scenarios);
  //c2e5879a-4b66-45f5-adfa-5385ed18ca0c
  
private:
  
  // #TODO Move to config !
  UPROPERTY()
  FString ServerURL = TEXT("wss://omni.itn.liu.se/ws/");
  UPROPERTY()
  FString ServerProtocol = TEXT("wss");

  
  TSharedPtr<IWebSocket> Socket; // The WebServer

  UPROPERTY()
  bool Enabled = false;
  UPROPERTY()
  bool Enabled2 = false;
};
