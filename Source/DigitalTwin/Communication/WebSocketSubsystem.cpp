#include "WebSocketSubsystem.h"
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

  //Load environment variables
  FString EnvContent;
  FFileHelper::LoadFileToString(EnvContent, *(FPaths::ProjectDir() / TEXT(".env")));
  TArray<FString> Lines;
  EnvContent.ParseIntoArrayLines(Lines);
  for (FString Line : Lines)
  {
    FString Key, Value;
    if (Line.Split(TEXT("="), &Key, &Value))
    {
      FPlatformMisc::SetEnvironmentVar(*Key, *Value);
    }
  }
  
  const UWebsocketSettings* Settings = GetMutableDefault<UWebsocketSettings>();
  UE_LOG(LogTemp, Display, TEXT("UWebSocketSubsystem Server   : %s "),*Settings->ServerURL);
  UE_LOG(LogTemp, Display, TEXT("UWebSocketSubsystem Protocol : %s "),*Settings->Protocol);

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
  JsonObject->SetStringField("token", FWindowsPlatformMisc::GetEnvironmentVariable(TEXT("TOKEN")));
  FString OutputString;
  TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
  FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);
  
  Socket->Send(OutputString);
  UE_LOG(LogTemp, Display, TEXT("UWebSocketSubsystem(FWebSocketWorker Init end ")); 
#endif
}

void UWebSocketSubsystem::Deinitialize() {
#ifdef USE_WEBSOCKET_SUBSYSTEM
  Super::Deinitialize();
  UE_LOG(LogTemp, Display, TEXT("UWebSocketSubsystem Deinitialize"));
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

void UWebSocketSubsystem::SendLayerResponse(FString Name, FString Error) const {
#ifdef USE_WEBSOCKET_SUBSYSTEM
  FLayerResponse Response;
  Response.Name = Name;
  Response.Error = Error;
  const FString JSONMessage = CreateJsonResponseString<FLayerResponse>(Response);
  SendJsonResponse(JSONMessage);
#endif
}

void UWebSocketSubsystem::SendResetResponse() const
{
  FResetResponse Response;
  const FString JSONMessage = CreateJsonResponseString<FResetResponse>(Response);
  SendJsonResponse(JSONMessage);
}

void UWebSocketSubsystem::SendRecacheResponse(const int Index, const int Max)
{
  FRecacheResponse Response;
  Response.Index = Index;
  Response.Max = Max;
  const FString JSONMessage = CreateJsonResponseString<FRecacheResponse>(Response);
  SendJsonResponse(JSONMessage);
}

void UWebSocketSubsystem::SendMapMarkerResponse(const TArray<FString> IDs) const
{
  FMapMarkerResponse Response;
  Response.IDs = IDs;
  const FString JSONMessage = CreateJsonResponseString<FMapMarkerResponse>(Response);
  SendJsonResponse(JSONMessage);
}

void UWebSocketSubsystem::HandleRequest(const FString &Message) const {
#ifdef USE_WEBSOCKET_SUBSYSTEM
  if (!Enabled) {
    return;
  }

  FRequest Req;
  FJsonObjectConverter::JsonObjectStringToUStruct(Message, &Req, 0, 0);
  FString JSONPayload = Message;
  
  if (Req.Type == "PingRequest") {
    FPingResponse Response;
    FString JSONMessage = CreateJsonResponseString<FPingResponse>(Response);
    if (Socket.IsValid()) {
      SendJsonResponse(JSONMessage);
    }
  }

  else if (Req.Type == "SetMapMarkerRequest") {
    FSetMapMarkerRequest Request =
        CreateRequestStruct<FSetMapMarkerRequest>(JSONPayload);
    OnMapMarkerUpdate.Broadcast(Request);
  }

  else if (Req.Type == "RemoveMapMarkerRequest")
  {
    FRemoveMapMarkerRequest Request = CreateRequestStruct<FRemoveMapMarkerRequest>(JSONPayload);
    OnMapMarkerRemove.Broadcast(Request);
  }

  else if (Req.Type == "LightRequest") {
    FLightRequest Response = CreateRequestStruct<FLightRequest>(JSONPayload);
    // #TODO Remove this ... ! ?
    FSkyLight2 SkyLight2;
    SkyLight2.Day = Response.Day;
    SkyLight2.Month = Response.Month;
    SkyLight2.SolarTime = Response.Solar_Time;
    SkyLight2.Year = Response.Year;
    OnLightUpdate.Broadcast(SkyLight2);
  }

  else if (Req.Type == "LayerRequest") {
    
    FLayerRequest Request = CreateRequestStruct<FLayerRequest>(JSONPayload);

    if (Request.Flush)
    {
      for (FLayerDef& Layer : Request.Layers)
      {
        if (Layer.Color.IsEmpty()){Layer.Color = "FFFFFF";}
      }
    }
    
    OnLayerUpdate.Broadcast(Request.Flush, Request.Layers);
  }

  else if (Req.Type == "RecacheRequest")
  {
    FRecacheRequest Request = CreateRequestStruct<FRecacheRequest>(JSONPayload);
    OnRecache.Broadcast(Request.Path);
  }

  else if (Req.Type == "ResetRequest") {
    FResetRequest Request =
        CreateRequestStruct<FResetRequest>(JSONPayload);
    // #TODO Remove this ... !
    OnReset.Broadcast(Request.Misc);
  }
  
  else
  {
    UE_LOG(LogTemp, Error, TEXT("Request Type Error: %s"), *Req.Type);
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

FLinearColor UWebSocketSubsystem::HexToColor(const FString& Hex) const
{
  FColor Color = FColor::FromHex(Hex);
  return FLinearColor(Color);
}

FString UWebSocketSubsystem::GetComputerName()
{
  return FString(FPlatformProcess::ComputerName());
}
