#include "ZeroMQCommunication.h"
#include "DigitalTwin/Datasets/ScenarioSubsystem.h"
#include "JsonObjectConverter.h"

void UZeroMqCommunication::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

// TODO : Review this one more time !!
void UZeroMqCommunication::Deinitialize()
{
	TArray<FString> Keys;
	Workers.GetKeys(Keys);

	for (const auto& Key : Keys)
	{
		const auto Worker = Workers[Key];
		if (Worker)
		{
			Worker->Stop();
			FPlatformProcess::Sleep(0.5f);
			Worker->Exit();
			FPlatformProcess::Sleep(0.5f);
			delete Worker;
		}
	}
}

// TODO : Note this maybe a bit of a problem
void UZeroMqCommunication::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (Worker_Queue.IsValid())
	{
		TArray<FTrafficData> Data_To_Send;
		while (!Worker_Queue->IsEmpty())
		{
			FTrafficData Temp;
			Worker_Queue->Dequeue(Temp);
			if (Temp.Color == FColor::Red)
			{
				UE_LOG(LogTemp, Warning, TEXT("Connected to tcp://localhost:5557 !!"));
			}
			OnTrafficUpdate.Broadcast(Temp);
		}
	}

	if (Message_Queue.IsValid())
	{
		while (!Message_Queue->IsEmpty())
		{
			FRequest* Temp;
			Message_Queue->Dequeue(Temp);
			if (FMapLightRequest* Req = static_cast<FMapLightRequest*>(Temp))
			{
				if (Req->Type == "MapLightRequest")
				{
					FMapLight Test;
					FLinearColor TempColor;
					TempColor.InitFromString(Req->Color);
					Test.Color = TempColor;
					Test.Id = Req->Name;
					Test.Pos = FVector2D(Req->Northing, Req->Easting);
					OnMapLightUpdate.Broadcast(Test);
				}
			}

			if (FLightRequest* Req = static_cast<FLightRequest*>(Temp))
			{
				if (Req->Type == "LightRequest")
				{
					FSkyLight2 Test;
					Test.Day = Req->Day;
					Test.Month = Req->Month;
					Test.SolarTime = Req->Solar_Time;
					Test.Year = Req->Year;
					OnLightUpdate.Broadcast(Test);
				}
			}

			if (FActiveDatasetRequest* Req = static_cast<FActiveDatasetRequest*>(Temp))
			{
				if (Req->Type == "ActiveDatasetRequest")
				{
					FActivateMap Test;
					Test.Datasets = Req->Datasets;
					OnDatasetUpdate.Broadcast(Test, true);
				}
			}

			if (FDeactiveDatasetRequest* Req = static_cast<FDeactiveDatasetRequest*>(Temp))
			{
				if (Req->Type == "DeactivateDatasetRequest")
				{
					FActivateMap Test;
					Test.Datasets = Req->Datasets;
					OnDatasetUpdate.Broadcast(Test, false);
				}
			}

			if (FRestApplication* Req = static_cast<FRestApplication*>(Temp))
			{
				if (Req->Type == "Reset")
				{
					OnReset.Broadcast(Req->Misc);
				}
			}

			UE_LOG(LogTemp, Warning, TEXT("Connected to tcp://localhost:5557 !!"));
		}
	}
}

TStatId UZeroMqCommunication::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UZeroMQCommunication, STATGROUP_Tickables);
}

void UZeroMqCommunication::StartWorker(const FString WorkerName,
                                       const FString IP_Adress)
{
	if (!Worker_Queue.IsValid())
	{
		Worker_Queue = MakeShared<TQueue<FTrafficData>>();
	}
	Workers.Add(WorkerName, new FZeroMqWorker(Worker_Queue, IP_Adress, WorkerName));
	Workers[WorkerName]->Init();
}

void UZeroMqCommunication::StartWebSocketWorker(FString WorkerName,
                                                FString Interface)
{
	if (!Message_Queue.IsValid())
	{
		Message_Queue = MakeShared<TQueue<FRequest*>>();
	}
	auto* Websocket_Worker =
		new FWebSocketWorker(Message_Queue, Interface, WorkerName,
		                     GetWorld()->GetSubsystem<UScenarioSubsystem>());

	if (Websocket_Worker)
	{
		Websocket_Worker->Init();
	}
	else
	{
		return;
	}

	WebWorkers.Add(WorkerName, Websocket_Worker);
}

void UZeroMqCommunication::StopWorker(const FString& WorkerName) const
{
	const auto Worker = Workers[WorkerName];
	if (Worker)
	{
		Worker->Stop();
	}
}

void UZeroMqCommunication::StopWorkers()
{
	{
		TArray<FString> Keys;
		Workers.GetKeys(Keys);

		for (const auto& Key : Keys)
		{
			const auto Worker = Workers[Key];

			if (Worker)
			{
				Worker->Stop();
				FPlatformProcess::Sleep(0.5f);
			}
		}
	}

	{
		TArray<FString> Keys;
		WebWorkers.GetKeys(Keys);
		for (const auto& Key : Keys)
		{
			const auto Worker = WebWorkers[Key];
			if (Worker)
			{
				Worker->Stop();
				FPlatformProcess::Sleep(1.0f);
			}
		}
	}
}

void UZeroMqCommunication::SendRequest(const FString WorkerName)
{
	const auto Worker = TempWorkers[WorkerName];
	if (Worker)
	{
		Worker->SendBlocksRequest("Test");
	}
}

void UZeroMqCommunication::SendWebSocketRequest(const FString WorkerName,
                                                FRequest Request)
{
	const auto Worker = WebWorkers[WorkerName];
	if (Worker)
	{
		// Worker->SendResponse();
	}
}

void UZeroMqCommunication::SendWebSocketResponse(const FString WorkerName,
                                                 FResponse Response)
{
	const auto Worker = WebWorkers[WorkerName];
	if (Worker)
	{
		// Convert to JSON ..... !
		// Worker->SendJSONResponse(nullptr);
	}
}
