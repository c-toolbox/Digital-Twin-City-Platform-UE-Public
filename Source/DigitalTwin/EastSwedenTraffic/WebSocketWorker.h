#pragma once

#include "CoreMinimal.h"
#include "HAL/Runnable.h"
#include "Utils.h"

#include "DigitalTwin/Datasets/ScenarioSubsystem.h"
#include "IWebSocket.h"       // Socket definition
#include "WebSocketsModule.h" // Module definition

//#include "WebSocketWorker.generated.h"

//UCLASS()
class DIGITALTWIN_API FWebSocketWorker final : public FRunnable {

  //GENERATED_BODY()
public:
  
  FWebSocketWorker(const TSharedPtr<TQueue<FRequest*>> &Q, const FString &IP,
                   const FString &Topic, UScenarioSubsystem *s);
  virtual ~FWebSocketWorker() override;

  // Begin FRunnable interface
  virtual bool Init() override;
  virtual uint32 Run() override;
  virtual void Stop() override;
  virtual void Exit() override;

  void HandleRequest(const FString &Message) const;
  void SendJSONResponse(const FString &JSONPayload) const;
  // End FRunnable interface
  void SetQueue(const TSharedPtr<TQueue<FTrafficData>> &Q);

private:
  //template<class T>
  //  T* createPointer(T &t);
  
  FRunnableThread *Thread;
  bool Running;
  // Your server URL. You can use ws, wss or wss+insecure.
  const FString ServerURL = TEXT("wss://omni.itn.liu.se/ws/");
  const FString ServerProtocol = TEXT("wss");
  TSharedPtr<IWebSocket> Socket; // The WebServer
  FString IP_Interface;
  TSharedPtr<TQueue<FRequest*>> Queue;
  UScenarioSubsystem *S_Subsystem;
};


