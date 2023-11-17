#pragma once

#include "CoreMinimal.h"
#include "HAL/Runnable.h"
#include <zmq.hpp>
#include "BlocksWorker.generated.h"

// TODO
USTRUCT(BlueprintType)
struct DIGITALTWIN_API FBlocksData {
  GENERATED_USTRUCT_BODY()
  UPROPERTY(BlueprintReadWrite)
  FString Id;
};

// TODO
class DIGITALTWIN_API FBlocksWorker final : public FRunnable {
public:
  FBlocksWorker(const FString &IP, const FString &Topic);
  virtual ~FBlocksWorker() override;
  // Begin FRunnable interface
  virtual bool Init() override;
  virtual uint32 Run() override;
  virtual void Stop() override;
  virtual void Exit() override;
  void SendBlocksRequest(const FString Message);

private:
  zmq::socket_t *Socket;
  FRunnableThread *Thread;
  bool bRunning;
  FString IP_Interface;
};
