#pragma once
#include "CoreMinimal.h"
#include "GeoReferencingSystem.h"
#include "EastSwedenTraffic.generated.h"

struct FTrafficData;
//TODO Update traffic struct !
USTRUCT()
struct FVisualTraffic {
	GENERATED_BODY()
		FVector NewPosition;
		int64 NewTimeStamp;
		FVector OldPosition;
		int64 OldTimeStamp;
		TArray<FVector> Positions;
		double CurrentTimeStamp;
		bool FirstUpdate = false;
		FColor Color;
		FString EntityType;
		FString Id;
};

UCLASS()
class DIGITALTWIN_API AEastSwedenTraffic : public AActor {
	GENERATED_BODY()	
public:
	AEastSwedenTraffic();
	virtual void OnConstruction(const FTransform& Transform) override;
	UFUNCTION(BlueprintCallable)
	virtual void Tick(float DeltaTime) override;
	void FromWgsToProjectionCoordinates(const FVector& WgsPosition, AGeoReferencingSystem* Subsystem, FVector& EngineCoordinates);
	void ToWgsFromProjectionCoordinates(const FVector& EngineCoordinates, AGeoReferencingSystem* Subsystem, FVector& WgsPosition);
	void CreateVisualEntity(const FTrafficData& Data, const FVector &EngineCoordinates, FVisualTraffic& Entity);
	void UpdateVisualEntity(const FTrafficData& Data, const FVector &EngineCoordinates, FVisualTraffic& Update_Candidate);
	// TODO : Maybe use a shapefile for this !!
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "East Sweden Traffic mesh")
	class UStaticMesh* TrafficMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "East Sweden Traffic mesh")
	TArray<class UHierarchicalInstancedStaticMeshComponent*> TrafficPositions;

	UPROPERTY()
	class USceneComponent* SceneRootComponent;

	UPROPERTY()
	class USceneComponent* MeshRootComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "East Sweden Traffic Actor")
	class UMaterial* MaterialBase;

protected:
	virtual void BeginPlay() override;
	void Update(const FTrafficData &Data);
private:
	void AddNewMesh(const FString &Type,const FString &Id,const FColor &Color ,const FVector &Position);
	void UpdateMesh(const FString &Type,const FString &Id, const FVector &Position);
	void RemoveMesh(const FString &Type,const FString &Id);
	
	UPROPERTY()
	TMap<FString,int32> TypeInstancePositions;
	UPROPERTY()
	TMap<FString,int32> IDInstancePositions;
	UPROPERTY()
	TMap<FString,FVisualTraffic> Traffic_DMap;
};
