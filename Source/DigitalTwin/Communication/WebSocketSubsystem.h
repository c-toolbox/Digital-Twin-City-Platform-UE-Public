#pragma once
#include "CoreMinimal.h"
#include "HAL/Runnable.h"
#include "WebSocketsModule.h" // Module definition
#include "IWebSocket.h" // Socket definition
#include "Subsystems/GameInstanceSubsystem.h"
#include "Subsystems/SubsystemCollection.h"
#include "Subsystems/WorldSubsystem.h"
#include "RequestResponse.h"
#include "WebSocketSubsystem.generated.h"


UCLASS(Config = Game)
class DIGITALTWIN_API UWebsocketSettings : public UObject {
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Config, Category = "UWebsocketSettings")
	FString ServerURL = TEXT("wss://omni.itn.liu.se/ws/");
	UPROPERTY(EditAnywhere, Config, Category = "WebsocketSettings")
	FString Protocol  = TEXT("wss");
	UPROPERTY(EditAnywhere, Config, Category = "WebsocketSettings")
	// FString Token  = TEXT("0545fc9a-258a-402d-a844-69063a00af58"); Test token (which we use downstairs)
	FString Token  = ""; //TEXT("c2e5879a-4b66-45f5-adfa-5385ed18ca0c");
};


UCLASS()
class DIGITALTWIN_API UWebSocketSubsystem : public UGameInstanceSubsystem {
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase &Collection) override;
	virtual void Deinitialize() override;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMapMarkerUpdate, const FSetMapMarkerRequest &, MapMarkerRequest);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMapMarkerRemove, const FRemoveMapMarkerRequest &, RemoveMapMarkerRequest);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLightUpdate, const FSkyLight2 &,Data);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FReset, const FString &, Misc);

	UDELEGATE()
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FLayerUpdate,const bool, Flush, const TArray<FLayerDef>&, Layers);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRecache, const FString, Path);
	
  
	UPROPERTY(BlueprintAssignable, Category = "DigitalTwin|WebSocketSubsystem")
	FLayerUpdate OnLayerUpdate;

	UPROPERTY(BlueprintAssignable, Category = "DigitalTwin|WebSocketSubsystem")
	FMapMarkerUpdate OnMapMarkerUpdate;

	UPROPERTY(BlueprintAssignable, Category = "DigitalTwin|WebSocketSubsystem")
	FMapMarkerRemove OnMapMarkerRemove;

	UPROPERTY(BlueprintAssignable, Category = "DigitalTwin|WebSocketSubsystem")
	FLightUpdate OnLightUpdate;

	UPROPERTY(BlueprintAssignable, Category = "DigitalTwin|WebSocketSubsystem")
	FReset OnReset;

	UPROPERTY(BlueprintAssignable, Category = "DigitalTwin|WebSocketSubsystem")
	FRecache OnRecache;

	UFUNCTION(BlueprintCallable, Category  = "DigitalTwin|WebSocketSubsystem")
	bool Enable();

	UFUNCTION(BlueprintCallable, Category  = "DigitalTwin|WebSocketSubsystem|Response")
	void SendLayerResponse(FString Name, FString Error) const;

	UFUNCTION(BlueprintCallable, Category  = "DigitalTwin|WebSocketSubsystem|Response")
	void SendResetResponse() const;

	UFUNCTION(BlueprintCallable, Category  = "DigitalTwin|WebSocketSubsystem|Response")
	void SendRecacheResponse(const int Index, const int Max);

	UFUNCTION(BlueprintCallable, Category  = "DigitalTwin|WebSocketSubsystem|Response")
	void SendMapMarkerResponse(TArray<FString> IDs) const;
  
	void HandleRequest(const FString &Message) const;
	void SendJsonResponse(const FString &ResponseString) const;

	UFUNCTION(BlueprintCallable, Category = "DigitalTwin|WebSocketSubsystem|DataConversion")
	FLinearColor HexToColor(const FString &Hex) const;

	//Util
	UFUNCTION(BlueprintPure, Category="System")
	static FString GetComputerName();

private:

	// The WebServer
	TSharedPtr<IWebSocket> Socket; 
	UPROPERTY()
	bool Enabled = false;
  
};


