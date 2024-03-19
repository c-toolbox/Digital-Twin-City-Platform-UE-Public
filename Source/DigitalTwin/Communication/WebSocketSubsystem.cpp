#include "WebSocketSubsystem.h"
#include "../Datasets/ScenarioSubsystem.h"
#include "JsonObjectConverter.h"


constexpr unsigned int Str2INT(const char* Str, const int H = 0)
{
  return !Str[H] ? 5381 : (Str2INT(Str, H+1) * 33) ^ Str[H];
}

#define USE_WEBSOCKET_SUBSYSTEM 

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
#ifdef USE_WEBSOCKET_SUBSYSTEM
  Super::Initialize(Collection);
  const UWebsocketSettings* Settings = GetMutableDefault<UWebsocketSettings>();
  UE_LOG(LogTemp, Display, TEXT(" | UWebSocketSubsystem Server   : %s "),*Settings->ServerURL);
  UE_LOG(LogTemp, Display, TEXT(" | UWebSocketSubsystem Protocol : %s "),*Settings->Protocol);

  if (!FModuleManager::Get().IsModuleLoaded("WebSockets")) {
    FModuleManager::Get().LoadModule("WebSockets");
    Socket = FWebSocketsModule::Get().CreateWebSocket(Settings->ServerURL, Settings->Protocol);
  } else {
    Socket = FWebSocketsModule::Get().CreateWebSocket(Settings->ServerURL, Settings->Protocol);
  }

  if (!Socket.IsValid()) {
    UE_LOG(LogTemp, Display, TEXT("Invalid socket pointer "));
    return;
  }

  UE_LOG(LogTemp, Display, TEXT(" UWebSocketSubsystem(FWebSocketWorker Init start !"));
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
  
  TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();
  //JsonObject->SetStringField("token", "0545fc9a-258a-402d-a844-69063a00af58");
  JsonObject->SetStringField("token", "c2e5879a-4b66-45f5-adfa-5385ed18ca0c");
  FString OutputString;
  TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
  FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

  // #TODO For some odd reason you can`t call SendJSONResponse here, hmmm .... !
  // .... ! Blame @Golen :-)
  
  Socket->Send(OutputString);
  UE_LOG(LogTemp, Display, TEXT("UWebSocketSubsystem(FWebSocketWorker Init end ")); 
#endif
}

void UWebSocketSubsystem::Deinitialize() {
#ifdef USE_WEBSOCKET_SUBSYSTEM
  Super::Deinitialize();
  UE_LOG(LogTemp, Display, TEXT("---UWebSocketSubsystem Deinitialize---"));
  Enabled = false;
#endif
}

bool UWebSocketSubsystem::Enable() {
#ifdef USE_WEBSOCKET_SUBSYSTEM
  if (Enabled) {
    Enabled = false;
    return Enabled;
  }
  Enabled = true;
  return Enabled;
#else
  return false;
#endif
}

bool UWebSocketSubsystem::DelayEnable() {
#ifdef USE_WEBSOCKET_SUBSYSTEM
  if (Enabled2) {
    Enabled2 = false;
    return Enabled2;
  }
  Enabled2 = true;
  return Enabled2;
#else
  return false;
#endif
}

void UWebSocketSubsystem::SendResponse(FString ResponseString) const {
#ifdef USE_WEBSOCKET_SUBSYSTEM
  FGenericResponse Response;
  Response.Type = ResponseString;
  Response.Misc = "";
  const FString JSONMessage = CreateJsonResponseString<FResponse>(Response);
  SendJsonResponse(JSONMessage);
#endif
}

void UWebSocketSubsystem::HandleRequest(const FString &Message) const {
#ifdef USE_WEBSOCKET_SUBSYSTEM
  if (!Enabled) {
    return;
  }

  FRequest Req;
  FJsonObjectConverter::JsonObjectStringToUStruct(Message, &Req, 0, 0);
  FString JSONPayload = Message;


  std::string test2 = std::string(TCHAR_TO_UTF8(*Req.Type));
  
  
  switch(Str2INT(test2.c_str())) {
    case Str2INT("ScenariosRequest"): {
      
    }
    case Str2INT("PingRequest"): {
      SendErrorResponse("ScenarioRequestError","Client haven`t specified language");
    }
  }

  
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
    // #TODO Remove this ... ! ?
    FMapLight Light;
    Light.Color = FLinearColor::FromSRGBColor(FColor::FromHex(Response.Color));
    Light.Id = Response.Name;
    Light.Pos = FVector2D(Response.Northing, Response.Easting);
    Light.Height = Response.Height;
    Light.TypeOfMessage = Response.TypeOfMessage;
    Light.Enabled = true;
    //OnMapLightUpdate.Broadcast(Light);
  }

  if (Req.Type == "LightRequest") {
    FLightRequest Response = CreateRequestStruct<FLightRequest>(JSONPayload);
    // #TODO Remove this ... ! ?
    FSkyLight2 SkyLight2;
    SkyLight2.Day = Response.Day;
    SkyLight2.Month = Response.Month;
    SkyLight2.SolarTime = Response.Solar_Time;
    SkyLight2.Year = Response.Year;
    OnLightUpdate.Broadcast(SkyLight2);
  }

  if (Req.Type == "ActivateDatasetRequest") {
    FActiveDatasetRequest Response =
        CreateRequestStruct<FActiveDatasetRequest>(JSONPayload);
    
    // Special Dataset
    if (Response.Datasets == "RiverFlow") {
      OnGenericActivation.Broadcast("RiverFlow",true);
    }
    
    //
    FActivateMap ActivationMap;
    ActivationMap.Datasets = Response.Datasets;
    OnDatasetUpdate.Broadcast(ActivationMap, true);
  }

  if (Req.Type == "DeactivateDatasetRequest") {
    FActiveDatasetRequest Response =
        CreateRequestStruct<FActiveDatasetRequest>(JSONPayload);
  
    // Special Dataset
    if(Response.Datasets == "RiverFlow") {
      OnGenericActivation.Broadcast("RiverFlow",false);
    }

    //
    FActivateMap ActivationMap;
    ActivationMap.Datasets = Response.Datasets;
    OnDatasetUpdate.Broadcast(ActivationMap, false);
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
#endif
}

void UWebSocketSubsystem::SendJsonResponse(const FString &JSONString) const {
#ifdef USE_WEBSOCKET_SUBSYSTEM
  if (!Socket.IsValid()) {
    return;
  }

  if (Socket->IsConnected()) {
    Socket->Send(JSONString);
  }
#endif
}

void UWebSocketSubsystem::SendErrorResponse(const FString &ResponseType,const FString &Message) const {
#ifdef USE_WEBSOCKET_SUBSYSTEM
  FErrorResponse Response;
  Response.Type = ResponseType;
  Response.Error = Message;
  FString JSONMessage = CreateJsonResponseString<FErrorResponse>(Response);
  SendJsonResponse(JSONMessage);
#endif
}


void UWebSocketSubsystem::SendScenarioJsonResponse(
    TArray<FScenario> Scenarios)  {
  
  #ifdef USE_WEBSOCKET_SUBSYSTEM
  if (!Socket.IsValid()) {
    return;
  }

  FServerSendAllScenariosResponse Response;
  // #TODO Add language in request
  Response.Scenarios = Scenarios;
  FString JSONMessage =
      CreateJsonResponseString<FServerSendAllScenariosResponse>(Response);
  SendJsonResponse(JSONMessage);
  #endif
}
