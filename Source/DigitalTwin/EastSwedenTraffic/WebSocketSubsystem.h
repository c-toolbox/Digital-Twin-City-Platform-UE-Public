#pragma once
#include "CoreMinimal.h"
#include "HAL/Runnable.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Subsystems/SubsystemCollection.h"
#include "Subsystems/WorldSubsystem.h"
#include "WebSocketsModule.h" // Module definition
#include "IWebSocket.h" // Socket definition
#include "Utils.h"
#include "WebSocketSubsystem.generated.h"


UCLASS()
class DIGITALTWIN_API UWebSocketSubsystem : public UGameInstanceSubsystem {
  GENERATED_BODY()

  //
  DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMapLightUpdate,
                                              const FMapLight &, Data);
  DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLightUpdate, const FSkyLight2 &,
                                              Data);
  DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FReset, const FString &, Misc);
  //
  DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDatasetUpdate,
                                               const FActivateMap &, Dataset,
                                               bool, Enable);
  
  //

public:
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

  void HandleRequest(const FString &Message) const;
  void SendJsonResponse(const FString &Message) const;

private:
  // #TODO Move to config !
  const FString ServerURL = TEXT("wss://omni.itn.liu.se/ws/");
  const FString ServerProtocol = TEXT("wss");
  TSharedPtr<IWebSocket> Socket; // The WebServer
};
