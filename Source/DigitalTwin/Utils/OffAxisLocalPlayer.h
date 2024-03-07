// Copyright 

#pragma once

#include "CoreMinimal.h"
#include "Engine/LocalPlayer.h"
#include "Runtime/Core/Public/Math/IntRect.h"
#include "OffAxisLocalPlayer.generated.h"

/**
 * Happily yoinked and modified code from https://github.com/fweidner/UE4-Plugin-OffAxis
 */
UCLASS(BlueprintType)
class DIGITALTWIN_API UOffAxisLocalPlayer : public ULocalPlayer
{
  GENERATED_BODY()
    FSceneView* CalcSceneView(class FSceneViewFamily* ViewFamily,
      FVector& OutViewLocation,
      FRotator& OutViewRotation,
      FViewport* Viewport,
      class FViewElementDrawer* ViewDrawer = NULL,
      EStereoscopicPass StereoPass = EStereoscopicPass::eSSP_FULL);

public:

  void UpdateProjectionMatrix(FSceneView* View, FMatrix OffAxisMatrix);

  FMatrix GenerateOffAxisMatrix(FVector ViewLocation);

  FMatrix FrustumMatrix(float left, float right, float bottom, float top, float nearVal, float farVal);

private:
  FMatrix mOffAxisMatrix = FMatrix();
  float OffAxisFarPlane = 10000.f;
  float OffAxisNearPlane = .1f;
};

//////////////////////////////////////////////////////////////////////////
static bool s_ShowDebugMessages = false;

//////////////////////////////////////////////////////////////////////////
static FVector screen_ll = FVector(-3931.f / 2.f, -5969.f / 2.f, 28.f);
static FVector screen_lr = FVector(-3931.f / 2.f, 5969.f / 2.f, 28.f);
static FVector screen_ul = FVector(3931.f / 2.f, -5969.f / 2.f, 28.f);
