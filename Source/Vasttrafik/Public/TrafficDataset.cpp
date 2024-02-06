// Fill out your copyright notice in the Description page of Project Settings.

#include "TrafficDataset.h"
#include "TrafficActor.h"

void UTrafficDataset::ActivateDataset(UWorld* World,int SortOrder, FString MaterialPath)
{
  TrafficActor->ToggleTraffic(true);
  TrafficActor->ActivateVehicleType(VehicleType);
}

void UTrafficDataset::ToggleDataset(UWorld* World)
{
  TrafficActor->ToggleTraffic(true);
  TrafficActor->ToggleVehicleType(VehicleType);
}

void UTrafficDataset::DisableDataset(UWorld* World)
{
  TrafficActor->ToggleTraffic(false);
  TrafficActor->DeactivateVehicleType(VehicleType);
}