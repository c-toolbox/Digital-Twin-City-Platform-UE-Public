// Copyright 


#include "OffAxisLocalPlayer.h"


#include "Engine.h"
#include "ProjectionSubsystem.h"

#include "Runtime/Engine/Classes/Engine/GameViewportClient.h"

FSceneView* UOffAxisLocalPlayer::CalcSceneView(FSceneViewFamily* ViewFamily, FVector& OutViewLocation, FRotator& OutViewRotation, FViewport* Viewport, FViewElementDrawer* ViewDrawer, EStereoscopicPass StereoPass)
{
  FSceneView* SceneView = ULocalPlayer::CalcSceneView(ViewFamily, OutViewLocation, OutViewRotation, Viewport, ViewDrawer, 0);
  if (SceneView)
  {
    const FMatrix OffAxisMatrix = GenerateOffAxisMatrix(SceneView->ViewLocation);
    UpdateProjectionMatrix(SceneView, OffAxisMatrix);
  }

  return SceneView;
}

void UOffAxisLocalPlayer::UpdateProjectionMatrix(FSceneView* View, FMatrix OffAxisMatrix)
{
  View->UpdateProjectionMatrix(OffAxisMatrix);
}

FMatrix UOffAxisLocalPlayer::GenerateOffAxisMatrix(FVector ViewLocation)
{
  FMatrix result;

  float l, r, b, t, n, f, nd;

  n = GNearClippingPlane;
  //HACK: This currently forces infinite far clipping plane
  f = n;

  //this is analog to: http://csc.lsu.edu/~kooima/articles/genperspective/
  //TODO: Could rotate the screen with a screen-rotator here (ndisplay does so):
  auto World = GetWorld();
  FVector pa, pb, pc;
  if (World) {
    auto ProjectionSubsystem = World->GetSubsystem<UProjectionSubsystem>();
    if (ProjectionSubsystem) {
      FProjectionScreen Screen = ProjectionSubsystem->GetProjectionScreen();
      pa = Screen.LowerLeftCorner;
      pb = Screen.LowerRightCorner;
      pc = Screen.UpperLeftCorner;
    }
  }
  else {
    //TODO: We truly don't need this fallback...
    pa = screen_ll;
    pb = screen_lr;
    pc = screen_ul;
  }


  FVector pe = FVector(ViewLocation.X, ViewLocation.Y, ViewLocation.Z);

  // TODO: Get this from a potentially rotated screen object in-scene instead of 
  // these static variables...
  // Compute the eye-to-screen corner vectors.
  FVector va = pa - pe;
  FVector vb = pb - pe;
  FVector vc = pc - pe;

  // Compute an orthonormal basis for the screen.
  FVector vr = pb - pa;
  vr.Normalize();
  FVector vu = pc - pa;
  vu.Normalize();
  FVector vn = -FVector::CrossProduct(vr, vu);
  vn.Normalize();

  // Find the distance from the eye to screen plane.
  float d = -FVector::DotProduct(va, vn);

  nd = n / d;

  // Find the extent of the perpendicular projection.
  l = FVector::DotProduct(vr, va) * nd;
  r = FVector::DotProduct(vr, vb) * nd;
  b = FVector::DotProduct(vu, va) * nd;
  t = FVector::DotProduct(vu, vc) * nd;

  // Load the perpendicular projection.
  result = FrustumMatrix(l, r, b, t, n, f);

  if (s_ShowDebugMessages)
  {
    GEngine->AddOnScreenDebugMessage(1, 2, FColor::Orange, FString::Printf(TEXT("pa: %s"), *pa.ToString()));
    GEngine->AddOnScreenDebugMessage(2, 2, FColor::Orange, FString::Printf(TEXT("pb: %s"), *pb.ToString()));
    GEngine->AddOnScreenDebugMessage(3, 2, FColor::Orange, FString::Printf(TEXT("pc: %s"), *pc.ToString()));
    GEngine->AddOnScreenDebugMessage(4, 2, FColor::Orange, FString::Printf(TEXT("pe: %s"), *pe.ToString()));
    GEngine->AddOnScreenDebugMessage(9, 2, FColor::Orange, FString::Printf(TEXT("Eye-Screen-Distance: %f"), d));
    GEngine->AddOnScreenDebugMessage(10, 4, FColor::Orange, FString::Printf(TEXT("nd: %f"), nd));
  }
  return result;
}

FMatrix UOffAxisLocalPlayer::FrustumMatrix(float l, float r, float b, float t, float n, float f)
{
  const float mx = 2.f * n / (r - l);
  const float my = 2.f * n / (t - b);
  const float ma = -(r + l) / (r - l);
  const float mb = -(t + b) / (t - b);

  // Support unlimited far plane (f==n)
  const float mc = (f == n) ? (1.0f - Z_PRECISION) : (f / (f - n));
  const float md = (f == n) ? (-n * (1.0f - Z_PRECISION)) : (-(f * n) / (f - n));

  const float me = 1.f;

  // Normal LHS
  const FMatrix ProjectionMatrix = FMatrix(
    FPlane(mx, 0, 0, 0),
    FPlane(0, my, 0, 0),
    FPlane(ma, mb, mc, me),
    FPlane(0, 0, md, 0));

  // Invert Z-axis (UE4 uses Z-inverted LHS)
  static const FMatrix flipZ = FMatrix(
    FPlane(1, 0, 0, 0),
    FPlane(0, 1, 0, 0),
    FPlane(0, 0, -1, 0),
    FPlane(0, 0, 1, 1));

  const FMatrix ResultMatrix(ProjectionMatrix * flipZ);

  return ResultMatrix;
}