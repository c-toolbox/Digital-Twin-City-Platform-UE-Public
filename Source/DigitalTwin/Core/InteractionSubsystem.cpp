// --------------------------------------------------------------------------------
// Copyright 2021, Linköping University and Universeum AB
// --------------------------------------------------------------------------------

#include "InteractionSubsystem.h"
#include "Framework/Application/SlateApplication.h"


void UInteractionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
  Super::Initialize(Collection);
  if (FSlateApplication::IsInitialized()) {
    FSlateApplication& SlateApp = FSlateApplication::Get();
    auto& InteractionUpdatedEvent = SlateApp.GetLastUserInteractionTimeUpdateEvent();

    InteractionUpdatedEvent.AddUObject(this, &UInteractionSubsystem::NewInteraction);
  }
}

void UInteractionSubsystem::Tick(float DeltaTime)
{
  Super::Tick(DeltaTime);
  FSlateApplication& SlateApp = FSlateApplication::Get();
  double LastInteractionTime = SlateApp.GetLastUserInteractionTime();
  double CurrentTime = SlateApp.GetCurrentTime();
  if (!bIsIdle && CurrentTime - LastInteractionTime > SecondsUntilIdle) {
    bIsIdle = true;
    OnDeviceIdle.Broadcast();
  }
}

TStatId UInteractionSubsystem::GetStatId() const 
{
  RETURN_QUICK_DECLARE_CYCLE_STAT(UInteractionSubsystem, STATGROUP_Tickables);
}

void UInteractionSubsystem::SetSecondsUntilIdle(float InSecondsUntilIdle)
{
  SecondsUntilIdle = InSecondsUntilIdle;
}

void UInteractionSubsystem::NewInteraction(double NewInteractionTime) {
  bIsIdle = false;
  OnNewInteraction.Broadcast();
}
