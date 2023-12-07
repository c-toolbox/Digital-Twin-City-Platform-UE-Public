#include "EastSwedenTraffic.h"
#include "ZeroMQCommunication.h"
#include "GeoReferencingSystem.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"

AEastSwedenTraffic::AEastSwedenTraffic(): TrafficMesh(nullptr) {
	PrimaryActorTick.bCanEverTick = true;
	SceneRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("EastSwedenTraffic Origin"));
	RootComponent      = SceneRootComponent;
	MeshRootComponent  = CreateDefaultSubobject<USceneComponent>(TEXT("EastSwedenTraffic Mesh Origin"));
	MeshRootComponent->SetupAttachment(RootComponent);
}

void AEastSwedenTraffic::OnConstruction(const FTransform& Transform) {
}

void AEastSwedenTraffic::BeginPlay() {
	Super::BeginPlay();
	UZeroMqCommunication* Zeromq_Subsystem = GetWorld()->GetSubsystem<UZeroMqCommunication>();
	if (Zeromq_Subsystem != nullptr) {
		Zeromq_Subsystem->OnTrafficUpdate.AddUObject(this, &AEastSwedenTraffic::Update);
	}
}

void AEastSwedenTraffic::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	
	//Get all types
	TArray<FString> TypeKeys;
	TypeInstancePositions.GetKeys(TypeKeys);
	AGeoReferencingSystem* Subsystem = AGeoReferencingSystem::GetGeoReferencingSystem(GetWorld());
	
	for (auto TypeKey : Traffic_DMap) {
		
		const auto Data = TypeKey.Value;
		auto Res = TypeInstancePositions.Num();
		
		if(Res == 0)
			continue;
		
		auto EntityType = Data.EntityType;
		auto Type = TypeInstancePositions.Find(EntityType);
		
		if(Type != nullptr) {
			if(Data.FirstUpdate)
				continue;
			
			auto T1 = static_cast<double>(Data.OldTimeStamp);
			auto T2 = static_cast<double>(Data.NewTimeStamp);
			
			auto Delta = T2 - T1;
			auto CurrentTime = Data.CurrentTimeStamp + Delta/10.0f;
			Traffic_DMap[Data.Id].CurrentTimeStamp = CurrentTime;
			
			if( (CurrentTime > T1) && (CurrentTime < T2) ) {
				/*
				if(Data.Positions.Num() == 4 )
				{
				/*
					q(t) = 0.5 *(   	(2 * P1) +
					(-P0 + P2) * t +
					(2*P0 - 5*P1 + 4*P2 - P3) * t2 +
					(-P0 + 3*P1- 3*P2 + P3) * t3)
				 */
					// auto CurrentTimeDelta = CurrentTime - T1;
					// const auto P0 = Data.Positions[0];
					// const auto P1 = Data.Positions[1];
					// const auto P2 = Data.Positions[2];
					// const auto P3 = Data.Positions[3];
					
				//	auto NewPos = 0.5f*((2*P1)+(-1.0f*P0+P2)*CurrentTimeDelta+(2*P0-5.0f*P1+4.0f*P2-P3)*CurrentTimeDelta*CurrentTimeDelta+(-P0+3*P1-3*P2+P3)*CurrentTimeDelta*CurrentTimeDelta*CurrentTimeDelta);
				//	UpdateMesh(EntityType,Data.Id,NewPos);	
				//} else {
				auto CurrentTimeDelta = CurrentTime - T1;
				auto PositionDelta   = Data.NewPosition - Data.OldPosition;
				const auto NewPos    = Data.OldPosition + (CurrentTimeDelta / Delta) * PositionDelta;
				UpdateMesh(EntityType,Data.Id,NewPos);	
				// }
			}
		}
	}
}


void AEastSwedenTraffic::FromWgsToProjectionCoordinates(const FVector& WgsPosition, AGeoReferencingSystem* Subsystem, FVector& EngineCoordinates)
{
	FGeographicCoordinates Geographic;
	Geographic.Latitude  = WgsPosition.Z;
	Geographic.Longitude = WgsPosition.Y;
	Geographic.Altitude  = 50.0f;
		
	FCartesianCoordinates Cartesian_Coordinates;
	Subsystem->GeographicToProjected(Geographic, Cartesian_Coordinates);
	Subsystem->ProjectedToEngine(Cartesian_Coordinates, EngineCoordinates);
		
	EngineCoordinates.X = EngineCoordinates.X / 100.0f;
	EngineCoordinates.Y = EngineCoordinates.Y / 100.0f;
	EngineCoordinates.Z = EngineCoordinates.Z / 100.0f;
}

void AEastSwedenTraffic::ToWgsFromProjectionCoordinates(const FVector& EngineCoordinates, AGeoReferencingSystem* Subsystem, FVector& WgsPosition)
{
	FVector CoordTobeTransformed;
	CoordTobeTransformed.X = EngineCoordinates.X * 100.0f;
	CoordTobeTransformed.Y = EngineCoordinates.Y * 100.0f;
	CoordTobeTransformed.Z = EngineCoordinates.Z * 100.0f;
	
	FCartesianCoordinates ProjectedCoordinates;
	Subsystem->EngineToProjected(CoordTobeTransformed,ProjectedCoordinates);
	
	FGeographicCoordinates WgsCoord;
	Subsystem->ProjectedToGeographic(ProjectedCoordinates,WgsCoord);

	WgsPosition.Z = WgsCoord.Latitude;
	WgsPosition.Y = WgsCoord.Longitude;
	//TODO Handle this better later !!
	WgsPosition.X	 = 50.f;
}

void AEastSwedenTraffic::CreateVisualEntity(const FTrafficData& Data, const FVector &EngineCoordinates, FVisualTraffic& Entity)
{
	Entity.Color = Data.Color;
	Entity.EntityType = Data.Type;
	Entity.NewPosition = EngineCoordinates;
	Entity.OldPosition = FVector::ZeroVector;
	Entity.NewTimeStamp = Data.TimeStamp;
	Entity.OldTimeStamp = Data.TimeStamp;
	Entity.CurrentTimeStamp =  static_cast<double>(Data.TimeStamp);
	Entity.FirstUpdate = true;
	Entity.Id = Data.Id;
}

void AEastSwedenTraffic::UpdateVisualEntity(const FTrafficData& Data, const FVector &EngineCoordinates, FVisualTraffic& Update_Candidate)
{
	UE_LOG(LogTemp, Log, TEXT("UpdateVisualEntity : Update_Candidate.Positions Size %d"),Update_Candidate.Positions.Num());
	const auto Oldpos = Update_Candidate.NewPosition;
	const auto OldTime = Update_Candidate.NewTimeStamp;
	
	Update_Candidate.OldPosition = Oldpos;
	Update_Candidate.OldTimeStamp = OldTime;
	Update_Candidate.CurrentTimeStamp =  static_cast<double>(OldTime);
	Update_Candidate.NewPosition = EngineCoordinates;
	Update_Candidate.NewTimeStamp = Data.TimeStamp;
	Update_Candidate.FirstUpdate = false;
	Update_Candidate.Id = Data.Id;
}

void AEastSwedenTraffic::Update(const FTrafficData &Data) {
	AGeoReferencingSystem* Subsystem = AGeoReferencingSystem::GetGeoReferencingSystem(GetWorld());
	if (Subsystem != nullptr) {
		if (Traffic_DMap.Find(Data.Id)) {

			if(Data.Status == "Remove") {
				RemoveMesh(Data.Type,Data.Id);
			}
			
			FVector EngineCoordinates;
			auto Update_Candidate = Traffic_DMap[Data.Id];
			FromWgsToProjectionCoordinates(Data.Position, Subsystem, EngineCoordinates);
			UpdateVisualEntity(Data, EngineCoordinates, Update_Candidate);
			Traffic_DMap[Data.Id] = Update_Candidate;
			
		} else {
			FVector EngineCoordinates;
			FromWgsToProjectionCoordinates(Data.Position, Subsystem, EngineCoordinates);
			FVisualTraffic Entity;
			CreateVisualEntity(Data, EngineCoordinates, Entity);
			Traffic_DMap.Add(Data.Id, Entity);
			AddNewMesh(Data.Type,Data.Id, Data.Color,EngineCoordinates);
		}
	} else {
		UE_LOG(LogTemp, Log, TEXT("Error : Could not find geo-subsystem !"));
	}
}


void AEastSwedenTraffic::AddNewMesh(const FString &Type,const FString &Id,const FColor &Color,const FVector &Position) {
	const auto TypeIndex = TypeInstancePositions.Find(Type);
	if( TypeIndex == nullptr) {
		const FName Instance_Name(Type);
		const auto Mesh =  NewObject<UHierarchicalInstancedStaticMeshComponent>(this,Instance_Name);
		
		Mesh->SetupAttachment(MeshRootComponent);
		Mesh->SetStaticMesh(TrafficMesh);
		
		const FLinearColor TypeColor(Color);
		UMaterialInstanceDynamic* DynamicMaterialInstance = UMaterialInstanceDynamic::Create(MaterialBase, this);
		DynamicMaterialInstance->SetVectorParameterValue({"Color"}, TypeColor);	
		
		//TODO Review this .. !
		Mesh->SetMaterial(0, DynamicMaterialInstance);
		FRotator Rotation;
		Rotation.Yaw = 0.5f;
		const FVector Scale(0.125f);
		//
		const auto Transform  = FTransform(Rotation,Position,Scale);
		const auto IdIdx = Mesh->AddInstance(Transform);
		//
		Mesh->SetWorldScale3D(Scale);
		IDInstancePositions.Add(Id,IdIdx);
		TrafficPositions.Push(Mesh);
		TypeInstancePositions.Add(Type,TrafficPositions.Num()-1);
		Mesh->RegisterComponent();
	} else {
		if(IDInstancePositions.Find(Id) == nullptr) {
			const auto Transform = FTransform(Position);
			const auto IdIdx = TrafficPositions[*TypeIndex]->AddInstance(Transform);
			IDInstancePositions.Add(Id,IdIdx);
		} else {
			UE_LOG(LogTemp, Error, TEXT(" Could not find id in IDinstancPositions ! "));	
		}
	}
}

void AEastSwedenTraffic::UpdateMesh(const FString& Type,const FString &Id, const FVector& Position) {
	const auto TypeIndex = TypeInstancePositions.Find(Type);
	if( TypeIndex == nullptr) {
		return;
	}
	const auto IdIndex = IDInstancePositions.Find(Id);
	if(IdIndex == nullptr) {
		UE_LOG(LogTemp, Error, TEXT(" Could not find Id in IDinstancPositions ! "));
		UE_LOG(LogTemp, Error, TEXT(" This shall not be possible ! "));	
		return;
	}
	const auto Mesh = TrafficPositions[*TypeIndex];
	const auto NewTransform = FTransform(Position);
	Mesh->UpdateInstanceTransform(*IdIndex,NewTransform,true,true);
}


void AEastSwedenTraffic::RemoveMesh(const FString &Type,const FString &Id)
{
	const auto TypeIndex = TypeInstancePositions.Find(Type);
	if( TypeIndex == nullptr) {
		return;
	}
	const auto IdIndex = IDInstancePositions.Find(Id);
	if(IdIndex == nullptr) {
		UE_LOG(LogTemp, Error, TEXT(" Could not find Id in IDinstancPositions ! "));
		UE_LOG(LogTemp, Error, TEXT(" This shall not be possible ! "));	
		return;
	}
	const auto Mesh = TrafficPositions[*TypeIndex];
	Mesh->RemoveInstance(*IdIndex);
}
