// --------------------------------------------------------------------------------
// Copyright 2021, Linköping University and Universeum AB
//
// Authors:
//   Erik Junholm, erik.junholm@liu.se
// --------------------------------------------------------------------------------

#include "Communication.h"
#include "../Datasets/ScenarioSubsystem.h"
#include "MessageEndpointBuilder.h"

// --------------------------------------------------------------------------------
//   Server Section
// --------------------------------------------------------------------------------

void UCommunicationServer::CreateCommunicationServer(UCommunicationServer * & Server)
{
    Server = NewObject<UCommunicationServer>();
}

void UCommunicationServer::StartupNetwork()
{
    MyEndpoint = FMessageEndpoint::Builder("DigitalTwinServer")
        // Attach message handler(s)
        .Handling<FClientAlivePing>(this, &UCommunicationServer::HandleClientAlivePing)
        .Handling<FClientGetAllDatasets>(this, &UCommunicationServer::HandleClientGetAllDatasets)
        .Handling<FClientGetAllScenarios>(this, &UCommunicationServer::HandleClientGetAllScenarios)
        .Handling<FClientActivateDatasets>(this, &UCommunicationServer::HandleClientGetActivateDatasets)
        .Handling<FScenarioSubSection>(this, &UCommunicationServer::HandleScenarioSubsection)
        .Handling<FSkyLightRequest>(this, &UCommunicationServer::HandleSkyLight)
        .Handling<FClientDeactivateDatasets>(this, &UCommunicationServer::HandleClientDeactivateAllDatasets)
        .Handling<FGenericCommand>(this, &UCommunicationServer::HandleClientGenericCommand)
        .Handling<FMapMarkerMessage>(this, &UCommunicationServer::HandleClientUpdateMapMarker);
    if (MyEndpoint.IsValid())
    {
        // Subscribe to message type.
        MyEndpoint->Subscribe<FClientAlivePing>();
        MyEndpoint->Subscribe<FClientGetAllDatasets>();
        MyEndpoint->Subscribe<FClientGetAllScenarios>();
        MyEndpoint->Subscribe<FClientActivateDatasets>();
        MyEndpoint->Subscribe<FMapMarkerMessage>();
        MyEndpoint->Subscribe<FScenarioSubSection>();
        MyEndpoint->Subscribe<FSkyLightRequest>();
        MyEndpoint->Subscribe<FGenericCommand>();
        
    }
    else {
        UE_LOG(LogTemp, Error, TEXT("Failed to build end-point"));
    }
}


void UCommunicationServer::ShutdownNetwork()
{
    MyEndpoint.Reset();
}


void UCommunicationServer::SendServerAlivePing(FString Info)
{
    if (MyEndpoint.IsValid())
    {
        MyEndpoint->Publish<FServerAlivePing>(new FServerAlivePing { Info });
    }
}


void UCommunicationServer::HandleClientAlivePing(const FClientAlivePing & Message,
                        const TSharedRef<IMessageContext, ESPMode::ThreadSafe> & Context)
{
    UE_LOG(LogTemp, Log, TEXT("UCommunicationServer: Received ClientAlivePing message"));
    OnClientAlivePing.Broadcast(Message);
}

void UCommunicationServer::HandleClientGetAllDatasets(const FClientGetAllDatasets & Message,
                        const TSharedRef<IMessageContext, ESPMode::ThreadSafe> & Context)
{
    UE_LOG(LogTemp, Log, TEXT("UCommunicationServer: Received HandleClienGetAllDatasets message s%"),*Message.Info);
    OnClientGetAllDatasetsDelegate.Broadcast(Message);
}

void UCommunicationServer::HandleClientGetAllScenarios(const FClientGetAllScenarios& Message,
    const TSharedRef<IMessageContext, ESPMode::ThreadSafe>& Context)
{
    UE_LOG(LogTemp, Log, TEXT("UCommunicationServer: Received HandleClientGetAllScenarios message s%"),*Message.Info);
    
    OnClientGetAllScenariosDelegate.Broadcast(Message);
}

void UCommunicationServer::HandleClientGetActivateDatasets(const FClientActivateDatasets& Message,
    const TSharedRef<IMessageContext, ESPMode::ThreadSafe>& Context)
{
    UE_LOG(LogTemp, Log, TEXT("UCommunicationServer: HandleClientGetActivateDatasets "));
    OnClientActivateDatasetsDelegate.Broadcast(Message);
}

void UCommunicationServer::HandleClientUpdateMapMarker(const FMapMarkerMessage& Message,
    const TSharedRef<IMessageContext, ESPMode::ThreadSafe>& Context)
{
    
    switch(Message.MessageType) {
    case MapMarkerRequestType::Add: {
        UE_LOG(LogTemp, Log, TEXT("UCommunicationServer: HandleClientUpdateMapMarker Add %s"),*Message.Name.ToString());
        break;
    }
    case MapMarkerRequestType::Delete: {
        UE_LOG(LogTemp, Log, TEXT("UCommunicationServer: HandleClientUpdateMapMarker Delete %s"),*Message.Name.ToString());
        break;
    }
    case MapMarkerRequestType::Update: {
        UE_LOG(LogTemp, Log, TEXT("UCommunicationServer: HandleClientUpdateMapMarker Update %s"),*Message.Name.ToString());
        break;
    }
    }
    OnClientUpdateMapMarkerDelegate.Broadcast(Message);
}

void UCommunicationServer::HandleScenarioSubsection(const FScenarioSubSection& Message,
    const TSharedRef<IMessageContext, ESPMode::ThreadSafe>& Context)
{
    OnClientSetScenarioSubsectionDelegate.Broadcast(Message);
}

void UCommunicationServer::HandleSkyLight(const FSkyLightRequest& Message,
    const TSharedRef<IMessageContext, ESPMode::ThreadSafe>& Context)
{
    OnClientSetSkyLightDelegate.Broadcast(Message);
}

void UCommunicationServer::HandleClientDeactivateAllDatasets(const FClientDeactivateDatasets& Message,
    const TSharedRef<IMessageContext, ESPMode::ThreadSafe>& Context)
{
    OnClientDisableDatasets.Broadcast(Message);
}

void UCommunicationServer::HandleClientGenericCommand(const FGenericCommand& Message,
    const TSharedRef<IMessageContext, ESPMode::ThreadSafe>& Context)
{
    OnGenericCommandDelegate.Broadcast(Message);
}

void UCommunicationServer::SendServerAllDatasets(TArray<FString> DataSetsName)
{
    UE_LOG(LogTemp, Log, TEXT("UCommunicationServer: Server Sends All Datasets "));
    if (MyEndpoint.IsValid())
    {
        auto Message = new FServerSendAllDatasets();
        Message->DataSets = DataSetsName; 
        MyEndpoint->Publish<FServerSendAllDatasets>(Message);
    }
}

void UCommunicationServer::SendServerAllScenarios(TArray<FScenario> Scenarios)
{
    if (MyEndpoint.IsValid()) {
        auto Message = new FServerSendAllScenarios();
        Message->Scenarios = Scenarios;
        MyEndpoint->Publish<FServerSendAllScenarios>(Message);
    }
}

// --------------------------------------------------------------------------------
//   Client Section
// --------------------------------------------------------------------------------

void UCommunicationClient::CreateCommunicationClient(UCommunicationClient * & Client)
{
    Client = NewObject<UCommunicationClient>();
}

void UCommunicationClient::StartupNetwork()
{
    MyEndpoint = FMessageEndpoint::Builder("DigitalTwinClient")
        // Attach message handlers
        .Handling<FServerAlivePing>(this, &UCommunicationClient::HandleServerAlivePing)
        .Handling<FServerSendAllDatasets>(this, &UCommunicationClient::HandleServerSendsAllDatasets)
        .Handling<FServerSendAllScenarios>(this, &UCommunicationClient::HandleServerSendsAllScenarios);
    
    if (MyEndpoint.IsValid())
    {
        // Subscribe to message type.
        MyEndpoint->Subscribe<FServerAlivePing>();
        MyEndpoint->Subscribe<FServerSendAllDatasets>();
        MyEndpoint->Subscribe<FServerSendAllScenarios>();
    }
    else {
        UE_LOG(LogTemp, Error, TEXT("Failed to build end-point"));
    }
}

void UCommunicationClient::ShutdownNetwork()
{
    MyEndpoint.Reset();
}

void UCommunicationClient::SendClientAlivePing(FString Info)
{
    if (MyEndpoint.IsValid())
    {
        MyEndpoint->Publish<FClientAlivePing>(new FClientAlivePing { Info });
    }
}

void UCommunicationClient::SendClientGetAllDataSets(FString Info)
{
    if (MyEndpoint.IsValid())
    {
        MyEndpoint->Publish<FClientGetAllDatasets>(new FClientGetAllDatasets { Info });
    }
}

void UCommunicationClient::SendClientGetAllScenarios(FClientGetAllScenarios Info)
{
    if (MyEndpoint.IsValid())
    {
        MyEndpoint->Publish<FClientGetAllScenarios>(new FClientGetAllScenarios { Info });
    } 
}

void UCommunicationClient::SendClientActivateDatasets(TArray<FString> Datasets)
{
    if (MyEndpoint.IsValid()) {
            MyEndpoint->Publish<FClientActivateDatasets>(new FClientActivateDatasets{Datasets});
    }
}

void UCommunicationClient::SendClientUpdateMapMarker(FMapMarkerMessage Marker)
{
    if (MyEndpoint.IsValid()) {
        MyEndpoint->Publish<FMapMarkerMessage>(new FMapMarkerMessage{Marker});
    }
}

void UCommunicationClient::SendClientScenarioSubsection(FScenarioSubSection SubSection) {
    
    if (MyEndpoint.IsValid()) {
        MyEndpoint->Publish<FScenarioSubSection>(new FScenarioSubSection{SubSection});
    }
}

void UCommunicationClient::SendClientSkyLightRequest(FSkyLightRequest SkyLight)
{
    if (MyEndpoint.IsValid()) {
        MyEndpoint->Publish<FSkyLightRequest>(new FSkyLightRequest{SkyLight});
    }
}

void UCommunicationClient::SendClientDisableAllDatasets()
{
    
}

void UCommunicationClient::SendClientGenericCommand(FGenericCommand SkyLight)
{
    if (MyEndpoint.IsValid()) {
        MyEndpoint->Publish<FGenericCommand>(new FGenericCommand{SkyLight});
    }
}

void UCommunicationClient::HandleServerAlivePing(const FServerAlivePing & Message,
                                                 const TSharedRef<IMessageContext, ESPMode::ThreadSafe> & Context)
{
    UE_LOG(LogTemp, Log, TEXT("UCommunicationClient: Received ServerAlivePing message"));
    OnServerAlivePing.Broadcast(Message);
}

void UCommunicationClient::HandleServerSendsAllDatasets(const FServerSendAllDatasets & Message,
                        const TSharedRef<IMessageContext, ESPMode::ThreadSafe> & Context)
{
    UE_LOG(LogTemp, Log, TEXT("UCommunicationClient: Received ServerSendAllDatasets message"));
    auto DataSetInfo = Message.DataSets;
    for(const auto& Dataset : DataSetInfo) {
        UE_LOG(LogTemp, Log, TEXT("UCommunicationClient: Received ServerSendAllDatasets Dataset Info : %s"),*Dataset);
        
    }
    OnServerSendsAllDataSet.Broadcast(Message);
}

void UCommunicationClient::HandleServerSendsAllScenarios(const FServerSendAllScenarios& Message,
    const TSharedRef<IMessageContext, ESPMode::ThreadSafe>& Context)
{
    auto ScenarioInfos = Message.Scenarios;
    for(const auto& ScenarioInfo : ScenarioInfos ) {
        UE_LOG(LogTemp, Log, TEXT("UCommunicationClient: Received SendsAllScenarios Dataset Info : %s"),*ScenarioInfo.Title);
        
    }
    OnServerSendAllScenarios.Broadcast(Message);
}
