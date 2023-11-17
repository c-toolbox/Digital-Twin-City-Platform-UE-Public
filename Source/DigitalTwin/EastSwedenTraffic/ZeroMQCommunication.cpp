#include "ZeroMQCommunication.h"

void UZeroMqCommunication::Initialize(FSubsystemCollectionBase &Collection) {
  Super::Initialize(Collection);
}

// TODO : Review this one more time !!
void UZeroMqCommunication::Deinitialize() {

  TArray<FString> Keys;
  Workers.GetKeys(Keys);

  for (const auto &Key : Keys) {
    const auto Worker = Workers[Key];
    if (Worker) {
      Worker->Stop();
      FPlatformProcess::Sleep(0.5f);
      Worker->Exit();
      FPlatformProcess::Sleep(0.5f);
      delete Worker;
    }
  }
}

// TODO : Note this maybe a bit of a problem
void UZeroMqCommunication::Tick(const float DeltaTime) {
  Super::Tick(DeltaTime);
  if (Worker_Queue.IsValid()) {
    TArray<FTrafficData> Data_To_Send;
    while (!Worker_Queue->IsEmpty()) {
      FTrafficData Temp;
      Worker_Queue->Dequeue(Temp);
      if (Temp.Color == FColor::Red) {
        UE_LOG(LogTemp, Warning, TEXT("Connected to tcp://localhost:5557 !!"));
      }
      OnTrafficUpdate2.Broadcast(Temp);
    }
  }
}

TStatId UZeroMqCommunication::GetStatId() const {
  RETURN_QUICK_DECLARE_CYCLE_STAT(UZeroMQCommunication, STATGROUP_Tickables);
}

void UZeroMqCommunication::StartWorker() {}

void UZeroMqCommunication::StartWorker2(const FString WorkerName,
                                        const FString IP_AdressString) {

  if (!Worker_Queue.IsValid()) {
    Worker_Queue = MakeShared<TQueue<FTrafficData>>();
  }

  const auto w = new FZeroMqWorker(Worker_Queue, IP_AdressString, WorkerName);
  w->Init();
  Workers.Add(WorkerName, w);
}

void UZeroMqCommunication::StopWorker(const FString &WorkerName) const {

  const auto Worker = Workers[WorkerName];
  if (Worker) {
    Worker->Stop();
  }
}

void UZeroMqCommunication::StopWorkers() {
  TArray<FString> Keys;
  Workers.GetKeys(Keys);
  for (const auto &Key : Keys) {
    const auto Worker = Workers[Key];
    if (Worker) {
      Worker->Stop();
      FPlatformProcess::Sleep(0.5f);
    }
  }
}

void UZeroMqCommunication::SendRequest(const FString WorkerName) {
  const auto Worker = TempWorkers[WorkerName];
  if(Worker) {
    Worker->SendBlocksRequest("Test");
  }
}

