#include "WebSocketSubsystem.h"
#include "../Datasets/ScenarioSubsystem.h"
#include "JsonObjectConverter.h"

template <typename T> T CreateRequestStruct(FString &JsonString) {
  T Response;
  FJsonObjectConverter::JsonObjectStringToUStruct(JsonString, &Response, 0, 0);
  UE_LOG(LogTemp, Display, TEXT("CreateRequestStruct !"));
  return Response;
}

template <typename T> FString CreateJsonResponseString(T &t) {
  FString Response;
  FJsonObjectConverter::UStructToJsonObjectString(T::StaticStruct(), &t,
                                                  Response, 0, 0);
  return Response;
}

void UWebSocketSubsystem::Initialize(FSubsystemCollectionBase &Collection) {
  Super::Initialize(Collection);
  UE_LOG(LogTemp, Display,
         TEXT("--------------------UWebSocketSubsystem "
              "Initialize--------------------"));
  if (!FModuleManager::Get().IsModuleLoaded("WebSockets")) {
    FModuleManager::Get().LoadModule("WebSockets");
    Socket =
        FWebSocketsModule::Get().CreateWebSocket(ServerURL, ServerProtocol);
  } else {
    Socket =
        FWebSocketsModule::Get().CreateWebSocket(ServerURL, ServerProtocol);
  }

  if (!Socket.IsValid()) {
    UE_LOG(LogTemp, Display, TEXT("Invalid socket pointer "));
    return;
  }

  UE_LOG(LogTemp, Display,
         TEXT(" UWebSocketSubsystem(FWebSocketWorker Init start !"));
  // We bind all available events
  Socket->OnConnected().AddLambda([]() -> void {
    UE_LOG(LogTemp, Display, TEXT("Succesfully connected to server"));
    // This code will run once connected.
  });

  Socket->OnConnectionError().AddLambda([](const FString &Error) -> void {
    // This code will run if the connection failed. Check Error to see what
    // happened.
    UE_LOG(LogTemp, Error, TEXT("Connection failed: %s"), *Error);
  });

  Socket->OnClosed().AddLambda(
      [](int32 StatusCode, const FString &Reason, bool bWasClean) -> void {
        // This code will run when the connection to the server has been
        // terminated. Because of an error or a call to Socket->Close().
        UE_LOG(LogTemp, Error, TEXT("Connection was terminated: %s"), *Reason);
      });

  Socket->OnMessage().AddLambda([this](const FString &Message) -> void {
    // This code will run when we receive a string message from the server.
    UE_LOG(LogTemp, Display, TEXT("Message received: %s"), *Message);
    HandleRequest(Message);
  });

  Socket->OnRawMessage().AddLambda(
      [](const void *Data, SIZE_T Size, SIZE_T BytesRemaining) -> void {
        // This code will run when we receive a raw (binary) message from the
        // server.
      });

  Socket->OnMessageSent().AddLambda([](const FString &MessageString) -> void {
    UE_LOG(LogTemp, Display, TEXT("OnMessageSent : %s"), *MessageString);
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

  // #TODO For some odd reason you can`t call SendJSONResponse here .... hmmm
  // ...
  // .... ! Blame @Golen :-)

  Socket->Send(OutputString);
  UE_LOG(LogTemp, Display,
         TEXT("UWebSocketSubsystem(FWebSocketWorker Init end "));
}

void UWebSocketSubsystem::Deinitialize() {
  Super::Deinitialize();
  UE_LOG(LogTemp, Display,
         TEXT("--------------------UWebSocketSubsystem "
              "Deinitialize--------------------"));
  Enabled = false;
}

bool UWebSocketSubsystem::Enable() {
  if (Enabled) {
    Enabled = false;
    return Enabled;
  } else {
    Enabled = true;
    return Enabled;
  }
}

bool UWebSocketSubsystem::DelayEnable() {
  if (Enabled2) {
    Enabled2 = false;
    return Enabled2;
  } else {
    Enabled2 = true;
    return Enabled2;
  }
}

void UWebSocketSubsystem::SendRepsonse(FString Message) {

  FGenericResponse Response;
  Response.Type = Message;
  Response.Misc = "";
  FString JSONMessage = CreateJsonResponseString<FResponse>(Response);
  SendJsonResponse(JSONMessage);
}

void UWebSocketSubsystem::HandleRequest(const FString &Message) const {

  if (!Enabled) {
    return;
  }

  FRequest Req;
  FJsonObjectConverter::JsonObjectStringToUStruct(Message, &Req, 0, 0);
  FString JSONPayload = Message;

  if (Req.Type == "ScenariosRequest") {
    // #TODO Maybe change this later

    FScenarioRequest Response =
        CreateRequestStruct<FScenarioRequest>(JSONPayload);
    if( (Response.Language != "sv-SE" ) && (Response.Language != "en-US") ) {
      SendErrorResponse("ScenarioRequestError","Client haven`t specified language");
    } else {
      OnGetScenarioRequest.Broadcast(Response.Language);  
    }
  }

  if (Req.Type == "PingRequest") {
    FPingResponse Response;
    FString JSONMessage = CreateJsonResponseString<FPingResponse>(Response);
    if (Socket.IsValid()) {
      SendJsonResponse(JSONMessage);
    }
  }

  if (Req.Type == "MapLightRequest") {
    FMapLightRequest Response =
        CreateRequestStruct<FMapLightRequest>(JSONPayload);
    // #TODO Remove this ... !
    FMapLight Test;
    Test.Color = FLinearColor::FromSRGBColor(FColor::FromHex(Response.Color));
    Test.Id = Response.Name;
    Test.Pos = FVector2D(Response.Northing, Response.Easting);
    Test.Height = Response.Height;
    Test.TypeOfMessage = Response.TypeOfMessage;
    Test.Enabled = true;
    OnMapLightUpdate.Broadcast(Test);
  }

  if (Req.Type == "LightRequest") {
    FLightRequest Response = CreateRequestStruct<FLightRequest>(JSONPayload);
    // #TODO Remove this ... !
    FSkyLight2 Test;
    Test.Day = Response.Day;
    Test.Month = Response.Month;
    Test.SolarTime = Response.Solar_Time;
    Test.Year = Response.Year;
    OnLightUpdate.Broadcast(Test);
  }

  if (Req.Type == "ActivateDatasetRequest") {
    FActiveDatasetRequest Response =
        CreateRequestStruct<FActiveDatasetRequest>(JSONPayload);

    // #TODO Remove this ... !
    FActivateMap Test;
    Test.Datasets = Response.Datasets;
    OnDatasetUpdate.Broadcast(Test, true);
  }

  if (Req.Type == "DeactivateDatasetRequest") {
    FActiveDatasetRequest Response =
        CreateRequestStruct<FActiveDatasetRequest>(JSONPayload);
    // #TODO Remove this ... !
    FActivateMap Test;
    Test.Datasets = Response.Datasets;
    OnDatasetUpdate.Broadcast(Test, false);
  }

  if (Req.Type == "ResetRequest") {
    FRestApplication Response =
        CreateRequestStruct<FRestApplication>(JSONPayload);
    // #TODO Remove this ... !
    OnReset.Broadcast(Response.Misc);
  }

  if (Req.Type == "ActivateTrafficRequest") {
    // #TODO Remove this ... !
    OnActivateRealTimeTraffic.Broadcast(true);
  }

  if (Req.Type == "DeactivateTrafficRequest") {
    // #TODO Remove this ... !
    OnActivateRealTimeTraffic.Broadcast(false);
  }
}

void UWebSocketSubsystem::SendJsonResponse(const FString &JSONString) const {
  
  if (!Socket.IsValid()) {
    return;
  }

  if (Socket->IsConnected()) {
    Socket->Send(JSONString);
  }
}

void UWebSocketSubsystem::SendErrorResponse(const FString &ResponseType,const FString &Message) const {
  FErrorResponse Response;
  Response.Type = ResponseType;
  Response.Error = Message;
  FString JSONMessage = CreateJsonResponseString<FErrorResponse>(Response);
  SendJsonResponse(JSONMessage);
}


void UWebSocketSubsystem::SendScenarioJsonRepsonse(
    TArray<FScenario> Scenarios)  {

  
  if (!Socket.IsValid()) {
    return;
  }

  FServerSendAllScenariosResponse Response;
  // #TODO Add language in request
  Response.Scenarios = Scenarios;
  FString JSONMessage =
      CreateJsonResponseString<FServerSendAllScenariosResponse>(Response);
  SendJsonResponse(JSONMessage);
}
