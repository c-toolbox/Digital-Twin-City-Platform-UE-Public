#include "WebSocketWorker.h"
#include "DigitalTwin/Datasets/ScenarioSubsystem.h"
#include "JsonObjectConverter.h"

#define GETENUMSTRING(etype, evalue)                                           \
  ((FindObject(ANY_PACKAGE, TEXT(etype), true) != nullptr)                     \
       ? FindObject(ANY_PACKAGE, TEXT(etype), true)                            \
             ->GetEnumName((int32)evalue)                                      \
       : FString("Invalid - are you sure enum uses UENUM() macro?"))

FWebSocketWorker::FWebSocketWorker(const TSharedPtr<TQueue<FRequest*>>& Q,
                                   const FString& IP, const FString& Topic,
                                   UScenarioSubsystem* S)
	: Running(false), IP_Interface(IP), Queue(Q), S_Subsystem(S)
{
	UE_LOG(LogTemp, Display, TEXT("<FWebSocketWorker(FWebSocketWorker start )>"),);


	if (!FModuleManager::Get().IsModuleLoaded("WebSockets"))
	{
		FModuleManager::Get().LoadModule("WebSockets");
		Socket = FWebSocketsModule::Get().CreateWebSocket(ServerURL, ServerProtocol);
		Thread = FRunnableThread::Create(this, TEXT("FWebSocketWorker : %s"));
	}
	else
	{
		Socket = FWebSocketsModule::Get().CreateWebSocket(ServerURL, ServerProtocol);
		Thread = FRunnableThread::Create(this, TEXT("FWebSocketWorker : %s"));
	}
	UE_LOG(LogTemp, Display, TEXT("<FWebSocketWorker(FWebSocketWorker end )>"),);
}

bool FWebSocketWorker::Init()
{
	if (!Socket.IsValid())
	{
		UE_LOG(LogTemp, Display, TEXT("Invalid socket pointer "));
		return false;
	}

	UE_LOG(LogTemp, Display, TEXT("<FWebSocketWorker(FWebSocketWorker Init start )>"));
	// We bind all available events
	Socket->OnConnected().AddLambda([]() -> void
	{
		UE_LOG(LogTemp, Display, TEXT("Succesfully connected to server"));
		// This code will run once connected.
	});

	Socket->OnConnectionError().AddLambda([](const FString& Error) -> void
	{
		// This code will run if the connection failed. Check Error to see what
		// happened.
		UE_LOG(LogTemp, Display, TEXT("Connection failed: %s"), *Error);
	});

	Socket->OnClosed().AddLambda([](int32 StatusCode, const FString& Reason,
	                                bool bWasClean) -> void
	{
		// This code will run when the connection to the server has been terminated.
		// Because of an error or a call to Socket->Close().
		UE_LOG(LogTemp, Display, TEXT("Connection was terminated: %s"), *Reason);
	});

	Socket->OnMessage().AddLambda([this](const FString& Message) -> void
	{
		// This code will run when we receive a string message from the server.
		UE_LOG(LogTemp, Display, TEXT("Message received: %s"), *Message);
		HandleRequest(Message);
	});

	Socket->OnRawMessage().AddLambda(
		[](const void* Data, SIZE_T Size, SIZE_T BytesRemaining) -> void
		{
			// This code will run when we receive a raw (binary) message from the
			// server.
		});

	Socket->OnMessageSent().AddLambda([](const FString& MessageString) -> void
	{
		// This code is called after we sent a message to the server.
	});

	// And we finally connect to the server.
	Socket->Connect();
	// #TODO Move to function !!
	TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();
	JsonObject->SetStringField("token", "c2e5879a-4b66-45f5-adfa-5385ed18ca0c");
	FString OutputString;
	TSharedRef<TJsonWriter<>> Writer =
		TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	// #TODO For some odd reason you can`t call SendJSONResponse here .... hmmm ... 
	// .... ! Blame @Golen :-)

	Socket->Send(OutputString);
	Running = true;
	return true;

	UE_LOG(LogTemp, Display, TEXT("<FWebSocketWorker(FWebSocketWorker Init end )>"));
}

uint32 FWebSocketWorker::Run()
{
	while (Running)
	{
		UE_LOG(LogTemp, Warning, TEXT("FWebSocketWorker::Run"));
		FPlatformProcess::Sleep(4.0f);
	}
	return 0;
}

void FWebSocketWorker::Stop()
{
	UE_LOG(LogTemp, Warning, TEXT("FWebSocketWorker thread is Stopping!"));
	Running = false;

	if (Socket.IsValid())
		Socket->Close();
}

void FWebSocketWorker::Exit()
{
	UE_LOG(LogTemp, Warning, TEXT("FWebSocketWorker thread is Exiting!"));
	// Socket.Reset();
}

FWebSocketWorker::~FWebSocketWorker()
{
	UE_LOG(LogTemp, Warning, TEXT("Connected"));
	if (Thread)
	{
		// Kill() is a blocking call, it waits for the thread to finish.
		// Hopefully that doesn't take too long
		Thread->Kill();
		delete Thread;
	}
}

//template <class T>
//T FWebSocketWorker::createPointer(T& t)
//{
//  return T;
//}

void FWebSocketWorker::HandleRequest(const FString& Message) const
{
	FRequest Req;
	FJsonObjectConverter::JsonObjectStringToUStruct(Message, &Req, 0, 0);

	if (Req.Type == "ScenariosRequest")
	{
		// #TODO Maybe change this later
		// Direct Response
		FString JSONPayload;
		FServerSendAllScenariosResponse Response;
		Response.Scenarios = S_Subsystem->GetScenarios(Language::Swedish);
		FJsonObjectConverter::UStructToJsonObjectString(
			FServerSendAllScenariosResponse::StaticStruct(), &Response, JSONPayload,
			0, 0);
		SendJSONResponse(JSONPayload);
	}

	if (Req.Type == "PingRequest")
	{
		// Direct Response
		FString JSONPayload;
		const FPingResponse Response;
		FJsonObjectConverter::UStructToJsonObjectString(
			FPingResponse::StaticStruct(), &Response, JSONPayload, 0, 0);
		SendJSONResponse(JSONPayload);
	}

	if (Req.Type == "MapLightRequest")
	{
		// Non-Direct response !
		FString JSONPayload;
		FMapLightRequest Response;
		FJsonObjectConverter::JsonObjectStringToUStruct(Message, &Response, 0, 0);
		// TODO Revie w this .... !
		// Leaves ownership to queue !!!
		FMapLightRequest* Res = new FMapLightRequest(Response);
		Queue->Enqueue(Res);
	}

	if (Req.Type == "LightRequest")
	{
		// Non-Direct response !
		FString JSONPayload;
		FLightRequest Response;
		FJsonObjectConverter::JsonObjectStringToUStruct(Message, &Response, 0, 0);
		// TODO Reveiw this .... !
		// Leaves ownership to queue !!!
		FLightRequest* Res = new FLightRequest(Response);
		Queue->Enqueue(Res);
	}

	if (Req.Type == "ActiveDatasetRequest")
	{
		// Non-Direct response !
		FString JSONPayload;
		FActiveDatasetRequest Response;
		FJsonObjectConverter::JsonObjectStringToUStruct(Message, &Response, 0, 0);
		// TODO Reveiw this .... !
		// Leaves ownership to queue !!!
		FActiveDatasetRequest* Res = new FActiveDatasetRequest(Response);
		Queue->Enqueue(Res);
	}

	if (Req.Type == "DeactivateDatasetRequest")
	{
		// Non-Direct response !
		FString JSONPayload;
		FDeactiveDatasetRequest Response;
		FJsonObjectConverter::JsonObjectStringToUStruct(Message, &Response, 0, 0);
		// TODO Reveiw this .... !
		// Leaves ownership to queue !!!
		FDeactiveDatasetRequest* Res = new FDeactiveDatasetRequest(Response);
		Queue->Enqueue(Res);
	}

	if (Req.Type == "Reset")
	{
		// Non-Direct response !
		FString JSONPayload;
		FRestApplication Response;
		FJsonObjectConverter::JsonObjectStringToUStruct(Message, &Response, 0, 0);
		// TODO Reveiw this .... !
		// Leaves ownership to queue !!!
		FRestApplication* Res = new FRestApplication(Response);
		Queue->Enqueue(Res);
	}
}

void FWebSocketWorker::SendJSONResponse(const FString& JSONPayload) const
{
	if (Socket->IsConnected())
	{
		{
			Socket->Send(JSONPayload);
		}
	}
}
