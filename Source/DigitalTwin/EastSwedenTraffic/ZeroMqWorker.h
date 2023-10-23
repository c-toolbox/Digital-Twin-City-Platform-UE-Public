#pragma once
#include <zmq.hpp>
#include "CoreMinimal.h"
#include "HAL/Runnable.h"
#include "ZeroMqWorker.generated.h"

USTRUCT(BlueprintType)
struct DIGITALTWIN_API FTrafficData {
	GENERATED_USTRUCT_BODY()
	UPROPERTY(BlueprintReadWrite)
		FString Id;
	UPROPERTY(BlueprintReadWrite)
		FString Type;
	UPROPERTY(BlueprintReadWrite)
		int32 TimeStamp;
	UPROPERTY(BlueprintReadWrite)
		FVector Position;
	UPROPERTY(BlueprintReadWrite)
		float Heading;
	UPROPERTY(BlueprintReadWrite)
		FColor Color;
	UPROPERTY(BlueprintReadWrite)
		float Radius;
	UPROPERTY(BlueprintReadWrite)
		FString Status;
};

//TODO

class DIGITALTWIN_API FZeroMqWorker final : public FRunnable {
public:
	FZeroMqWorker(const TSharedPtr<TQueue<FTrafficData>>& Q, const FString &IP, const FString &Topic);
	virtual ~FZeroMqWorker() override;

	// Begin FRunnable interface
	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;
	virtual void Exit() override;

	// End FRunnable interface
	void HandlePublicTraffic(const std::string &Message_Str) const;
	void HandleImageOther(const std::string& Message_Str) const;
	void SetTopic(const FString &Topic);
	void SetQueue(const TSharedPtr<TQueue<FTrafficData>>& Q);
	
private:
	void SetUpTopics();

	FRunnableThread* Thread;
	bool Running;
	
	zmq::socket_t* Socket;
	FString IP_Interface;
	
	TSharedPtr<TQueue<FTrafficData>> Queue;
	TArray<FString> TopicStrings;
};





