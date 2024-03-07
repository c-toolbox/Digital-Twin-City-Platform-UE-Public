// Fill out your copyright notice in the Description page of Project Settings.

#include "Metadata.h"
#include "GenericPlatform/GenericPlatformFile.h"

#include "JsonObjectConverter.h"

FRasterDataRow FRasterDataRow::ParseFromJsonFile(const FString& JsonFileName)
{
  FRasterDataRow RasterDataTable;
  if (!FPaths::FileExists(JsonFileName))
  {
    UE_LOG(LogTemp, Warning, TEXT("JSON Metadata file does not exist: %s"), *JsonFileName);
    return RasterDataTable;
  }
  FString JsonString;
  if (FFileHelper::LoadFileToString(JsonString, *JsonFileName))
  {
    TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
    TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonString);
    //TODO: Split this up into a helper serde-class?
    if (FJsonSerializer::Deserialize(JsonReader, JsonObject) && JsonObject.IsValid()) {
      if (FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), FRasterDataRow::StaticStruct(), &RasterDataTable, 0, 0)) {
        UE_LOG(LogTemp, Log, TEXT("Succeeded in deserializing raster datatable\n Name: %s, Min: %f/%f"), *RasterDataTable.Name, RasterDataTable.Extent.Min.X, RasterDataTable.Extent.Min.Y);
      }
    }
  }
  return RasterDataTable;
}
