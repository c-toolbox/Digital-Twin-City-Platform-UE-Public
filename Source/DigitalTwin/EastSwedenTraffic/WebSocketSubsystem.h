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


UCLASS(Config = Game)
class DIGITALTWIN_API UWebsocketSettings : public UObject {
  GENERATED_BODY()
public:

  //
  // Values are from test configuration in Gothenburg !
  //
  UPROPERTY(EditAnywhere, Config, Category = "UWebsocketSettings")
  FString ServerURL = TEXT("wss://omni.itn.liu.se/ws/");
  UPROPERTY(EditAnywhere, Config, Category = "WebsocketSettings")
  FString Protocol  = TEXT("wss");;
};


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

  DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FGenericActivation, const FString &, Message ,bool , OnOff);
  
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

  UPROPERTY(BlueprintAssignable, Category = "DigitalTwin|WebSocketSubsystem")
  FGenericActivation OnGenericActivation;

  UFUNCTION(BlueprintCallable, Category  = "DigitalTwin|WebSocketSubsystem")
  bool Enable();

  UFUNCTION(BlueprintCallable, Category  = "DigitalTwin|WebSocketSubsystem")
  bool DelayEnable();

  UFUNCTION(BlueprintCallable, Category  = "DigitalTwin|WebSocketSubsystem")
  void SendResponse(FString Response) const;
  
  UFUNCTION(BlueprintCallable, Category = "DigitalTwin|WebSocketSubsystem")
  void SendScenarioJsonResponse(TArray<FScenario> Scenarios);
  
  void HandleRequest(const FString &Message) const;
  void SendJsonResponse(const FString &ResponseString) const;
  void SendErrorResponse(const FString &ResponseType,const FString &Message) const;

private:
  
  TSharedPtr<IWebSocket> Socket; // The WebServer

  UPROPERTY()
  bool Enabled = false;
  UPROPERTY()
  bool Enabled2 = false;
};
