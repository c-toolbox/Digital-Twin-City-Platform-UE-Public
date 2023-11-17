#include "RasterActorHandler.h"

ARasterActorHandler::ARasterActorHandler(): Super() {
   // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
   PrimaryActorTick.bCanEverTick = true;
 }

void ARasterActorHandler::BeginPlay() {
   Super::BeginPlay();
 }

void ARasterActorHandler::Tick(float DeltaTime) {
   // Called every frame...!
   Super::Tick(DeltaTime);
 }


void ARasterActorHandler::OnConstruction(const FTransform& Transform) {
   Super::OnConstruction(Transform);
   //TBD: If we always use the internal centimeter-scale this transformation is not needed.
 }