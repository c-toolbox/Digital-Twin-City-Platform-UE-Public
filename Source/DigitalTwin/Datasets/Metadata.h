// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CartesianCoordinates.h"
#include "Engine/DataTable.h"


#include "Metadata.generated.h"

USTRUCT(BlueprintType)
struct FPoint2D
{
  GENERATED_BODY()
public:
  
  UPROPERTY(EditAnywhere)
    double X;
  UPROPERTY(EditAnywhere)
    double Y;
};

USTRUCT(BlueprintType)
struct FExtent
{
  GENERATED_BODY()
public:
  UPROPERTY(EditAnywhere)
    FPoint2D Max;
  UPROPERTY(EditAnywhere)
    FPoint2D Min;
};

USTRUCT()
struct DIGITALTWIN_API FRasterDataRow : public FTableRowBase
{
  GENERATED_BODY()
public:
  static FRasterDataRow ParseFromJsonFile(const FString& JsonFileName);

  UPROPERTY(EditAnywhere)
    FExtent Extent;
  UPROPERTY(EditAnywhere)
    FString Name;
  UPROPERTY(EditAnywhere)
    UTexture2D* RasterTexture;
};

USTRUCT()
struct DIGITALTWIN_API FMediaDataRow : public FTableRowBase
{
  GENERATED_BODY()
public:

  UPROPERTY(EditAnywhere)
    FExtent Extent;
  UPROPERTY(EditAnywhere)
    FString Name;
  UPROPERTY(EditAnywhere)
    FRotator Rotation;
  UPROPERTY(EditAnywhere)
    class UMediaTexture* MediaTexture;
  UPROPERTY(EditAnywhere)
    class UMediaSource* MediaSource;
  UPROPERTY(EditAnywhere)
    class UMediaPlayer* MediaPlayer;
};

