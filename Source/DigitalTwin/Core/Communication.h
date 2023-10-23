// --------------------------------------------------------------------------------
// Copyright 2021, Linköping University and Universeum AB
//
// Authors:
//   Erik Junholm, erik.junholm@liu.se
//
// Description:
//   
//
// Package:
//  
//  
// Background:
//   This code was originally developed in the Vislabs project
//   
// --------------------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "MessageEndpoint.h"
#include "VectorTypes.h"
#include "../DatasetUtils/ScenarioMetadata.h"
#include "Communication.generated.h"

/**
 *
 *
 *
 */
USTRUCT(BlueprintType, Category = Communication)
struct FClientAlivePing
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Communication)
    FString Info;
};

/**
 *
 *
 *
 */
USTRUCT(BlueprintType, Category = Communication)
struct FClientGetAllDatasets
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Communication)
    FString Info;
};

/**
 *
 *
 *
 */
USTRUCT(BlueprintType, Category = Communication)
struct FClientActivateDatasets
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Communication)
    TArray<FString> DataSets;
};

/**
 *
 *
 *
 */
USTRUCT(BlueprintType, Category = Communication)
struct FClientDeactivateDatasets
{
    GENERATED_BODY()
public:
    
    /// Not used (for now)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Communication)
    TArray<FString> DataSets;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Communication)
    bool Enable;
};

USTRUCT(BlueprintType, Category = Communication)
struct FClientScenarioSubsectionDatasets
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Communication)
    TArray<FString> DataSets;
};

/**
 *
 *
 *
 */
USTRUCT(BlueprintType, Category = Communication)
struct FClientGetAllScenarios
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Communication)
    FString Info;
};

/**
 *
 *
 *
 */
USTRUCT(BlueprintType, Category = Communication)
struct FServerAlivePing
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Communication)
    FString Info;
};

/**
 *
 *
 *
 */
USTRUCT(BlueprintType, Category = Communication)
struct FServerSendAllDatasets
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Communication)
    TArray<FString> DataSets;
};

/**
 *
 *
 *
 */
USTRUCT(BlueprintType, Category = Communication)
struct FServerSendAllScenarios
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Communication)
    TArray<FScenario> Scenarios;
};

/**
 *
 *
 *
 */
UENUM(BlueprintType, Category = Communication)
enum MapMarkerRequestType
{
    Add     UMETA(DisplayName = "Add"),
    Update  UMETA(DisplayName = "Update"),
    Delete  UMETA(DisplayName = "Delete"),
};

/**
 *
 *
 *
 */
USTRUCT(BlueprintType, Category = Communication)
struct FMapMarkerMessage
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Communication)
    FVector2D Pos;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Communication)
    FLinearColor Color;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Communication)
    FName Name;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Communication)
    TEnumAsByte<MapMarkerRequestType> MessageType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Communication)
    bool Enabled;
};

/**
 *
 *
 *
 */
UENUM(BlueprintType, Category = Communication)
enum SkyLightRequestType {
    Day UMETA(DisplayName = "Day"),
    Dusk UMETA(DisplayName = "Dusk"),
    Night UMETA(DisplayName = "Night"),
};

/**
 *
 *
 */
USTRUCT(BlueprintType, Category = Communication)
struct FSkyLightRequest
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Communication)
    TEnumAsByte<SkyLightRequestType> MessageType;

};


USTRUCT(BlueprintType, Category = Communication)
struct FPointLayerRequest
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Communication)
    TEnumAsByte<SkyLightRequestType> MessageType;

};


/**
 *
 *
 */
USTRUCT(BlueprintType, Category = Communication)
struct FGenericCommand
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Communication)
    FString Command;
};


/**
 * Communication Server
 */
UCLASS(BlueprintType)
class DIGITALTWIN_API UCommunicationServer : public UObject
{
	GENERATED_BODY()

public:
    /// Create a Communication Server instance
    UFUNCTION(BlueprintCallable, Category = "Communication")
    static void CreateCommunicationServer(UCommunicationServer * & Server);

    /// Startup Networking for Server
    UFUNCTION(BlueprintCallable, Category = Communication)
    void StartupNetwork();
    /// Shutdown Networking for Server
    UFUNCTION(BlueprintCallable, Category = Communication)
    void ShutdownNetwork();

    /// Broadcast a Server Alive Ping
    UFUNCTION(BlueprintCallable, Category = Communication)
    void SendServerAlivePing(FString Info);

    /// Broadcast a Server Alive Ping
    UFUNCTION(BlueprintCallable, Category = Communication)
    void SendServerAllDatasets(TArray<FString> DataSetsName);

    UFUNCTION(BlueprintCallable, Category = Communication)
    void SendServerAllScenarios(TArray<FScenario> Scenarios);
    
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FClientAlivePingDelegate,
                                                const FClientAlivePing &, Message);

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FClientGetAllDatasetsDelegate,
                                                const FClientGetAllDatasets&, Datasets);
    
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FClientGetAllScenariosDelegate,
                                                const FClientGetAllScenarios&, Datasets);

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FClientActivateDatasetsDelegate,
                                                const FClientActivateDatasets&, Datasets);

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FClientUpdateMapMarkerDelegate,
                                                const FMapMarkerMessage&, Marker);

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FClientSetScenarioSubsectionDelegate,
                                                const FScenarioSubSection&, Subsection);

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FClientSetSkyLightDelegate,
                                                const FSkyLightRequest&, SkyLightRequest);

    /// Disables all datasets (For now !!)
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FClientDisableDatasets,
                                                const FClientDeactivateDatasets&, Datasets);

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGenericCommandDelegate,
                                                const FGenericCommand&, Command);
    
    /// Delegate for Client Alive Ping message received
    UPROPERTY(BlueprintAssignable)
    FClientAlivePingDelegate OnClientAlivePing;

    UPROPERTY(BlueprintAssignable)
    FClientGetAllDatasetsDelegate OnClientGetAllDatasetsDelegate;

    UPROPERTY(BlueprintAssignable)
    FClientGetAllScenariosDelegate OnClientGetAllScenariosDelegate;

    UPROPERTY(BlueprintAssignable)
    FClientActivateDatasetsDelegate OnClientActivateDatasetsDelegate;

    UPROPERTY(BlueprintAssignable)
    FClientUpdateMapMarkerDelegate OnClientUpdateMapMarkerDelegate;

    UPROPERTY(BlueprintAssignable)
    FClientSetScenarioSubsectionDelegate OnClientSetScenarioSubsectionDelegate;
    
    UPROPERTY(BlueprintAssignable)
    FClientSetSkyLightDelegate OnClientSetSkyLightDelegate;

    UPROPERTY(BlueprintAssignable)
    FClientDisableDatasets OnClientDisableDatasets;

    UPROPERTY(BlueprintAssignable)
    FGenericCommandDelegate OnGenericCommandDelegate;

protected:
    
    void HandleClientAlivePing(const FClientAlivePing & Message,
                               const TSharedRef<IMessageContext, ESPMode::ThreadSafe> & Context);

    void HandleClientGetAllDatasets(const FClientGetAllDatasets & Message,
                               const TSharedRef<IMessageContext, ESPMode::ThreadSafe> & Context);
    
    void HandleClientGetAllScenarios(const FClientGetAllScenarios & Message,
                               const TSharedRef<IMessageContext, ESPMode::ThreadSafe> & Context);

    void HandleClientGetActivateDatasets(const FClientActivateDatasets & Message,
                               const TSharedRef<IMessageContext, ESPMode::ThreadSafe> & Context);

    void HandleClientUpdateMapMarker(const FMapMarkerMessage & Message,
                               const TSharedRef<IMessageContext, ESPMode::ThreadSafe> & Context);

    void HandleScenarioSubsection(const FScenarioSubSection & Message,
                               const TSharedRef<IMessageContext, ESPMode::ThreadSafe> & Context);

    void HandleSkyLight(const FSkyLightRequest & Message,
                              const TSharedRef<IMessageContext, ESPMode::ThreadSafe> & Context);

    void HandleClientDeactivateAllDatasets(const FClientDeactivateDatasets & Message,
                              const TSharedRef<IMessageContext, ESPMode::ThreadSafe> & Context);

    void HandleClientGenericCommand(const FGenericCommand & Message,
                              const TSharedRef<IMessageContext, ESPMode::ThreadSafe> & Context);
    
    
    TSharedPtr<FMessageEndpoint, ESPMode::ThreadSafe> MyEndpoint;
};




/**
 * Communication Client
 */
UCLASS(BlueprintType)
class DIGITALTWIN_API UCommunicationClient : public UObject
{
    GENERATED_BODY()

public:
    /// Create a Communication Client instance
    UFUNCTION(BlueprintCallable, Category = "Communication")
    static void CreateCommunicationClient(UCommunicationClient * & Client);

    /// Startup the Networking for this instance
    UFUNCTION(BlueprintCallable, Category = Communication)
    void StartupNetwork();
    /// Shutdown the Networking for this instance
    UFUNCTION(BlueprintCallable, Category = Communication)
    void ShutdownNetwork();

    /// Broadcast a Client Alive Ping
    UFUNCTION(BlueprintCallable, Category = Communication)
    void SendClientAlivePing(FString Info);

    /// Broadcast request for all datasets
    UFUNCTION(BlueprintCallable, Category = Communication)
    void SendClientGetAllDataSets(FString Info);

    /// Broadcast request for all scenarios
    UFUNCTION(BlueprintCallable, Category = Communication)
    void SendClientGetAllScenarios(FString Info);
    
    /// Broadcast request for activation of datasets
    UFUNCTION(BlueprintCallable, Category = Communication)
    void SendClientActivateDatasets(TArray<FString> Datasets);
    
    /// Broadcast update for light
    UFUNCTION(BlueprintCallable, Category = Communication)
    void SendClientUpdateMapMarker(FMapMarkerMessage Marker);

    /// Broadcast request for activation of scenario subsection
    UFUNCTION(BlueprintCallable, Category = Communication)
    void SendClientScenarioSubsection(FScenarioSubSection SubSection);

    /// Broadcast request for activation of scenario subsection
    UFUNCTION(BlueprintCallable, Category = Communication)
    void SendClientSkyLightRequest(FSkyLightRequest SkyLight);

    /// Broadcast request for deactivation of scenario subsections
    UFUNCTION(BlueprintCallable, Category = Communication)
    void SendClientDisableAllDatasets();

    /// Broadcast request for activation of scenario subsection
    UFUNCTION(BlueprintCallable, Category = Communication)
    void SendClientGenericCommand(FGenericCommand SkyLight);
    
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FServerAlivePingDelegate,
                                                const FServerAlivePing &, Message);

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FServerSendsAllDatasetsDelegate,
                                                const FServerSendAllDatasets &, Message);

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FServerSendsAllScenariosDelegate,
                                                const FServerSendAllScenarios &, Message);
    

    /// Delegate for Server Alive Ping message received
    UPROPERTY(BlueprintAssignable)
    FServerAlivePingDelegate OnServerAlivePing;

    UPROPERTY(BlueprintAssignable)
    FServerSendsAllDatasetsDelegate OnServerSendsAllDataSet;
    
    UPROPERTY(BlueprintAssignable)
    FServerSendsAllScenariosDelegate OnServerSendAllScenarios;

protected:
    void HandleServerAlivePing(const FServerAlivePing & Message,
                               const TSharedRef<IMessageContext, ESPMode::ThreadSafe> & Context);

    void HandleServerSendsAllDatasets(const FServerSendAllDatasets & Message,
                    const TSharedRef<IMessageContext, ESPMode::ThreadSafe> & Context);

    void HandleServerSendsAllScenarios(const FServerSendAllScenarios & Message,
                    const TSharedRef<IMessageContext, ESPMode::ThreadSafe> & Context);

    TSharedPtr<FMessageEndpoint, ESPMode::ThreadSafe> MyEndpoint;
};
