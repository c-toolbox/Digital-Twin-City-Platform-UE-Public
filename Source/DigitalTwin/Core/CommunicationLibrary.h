// --------------------------------------------------------------------------------
// Copyright 2021, Linköping University and Universeum AB
//
// Authors:
//   Patric Ljung, patric.ljung@liu.se
//
// Description:
//   The Map Visualization tools provides a set of tools and actors to visualize
//   data of a country at three levels, Nation, County, Municipalities.
//
// Package:
//   VEINS/MapVisualization (Blueprints and things)
//   VEIMS/MapVisualizationSettings (Configuration of the map)
//   VEINS/MapVisualizationUtilities (types and blueprint library)
//
// Background:
//   This code was originally developed in the Vislabs project for the
//   VEINS application.
// --------------------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CommunicationLibrary.generated.h"

class UCommunicationServer;
class UCommunicationClient;

/**
 * Communication Blueprint Library
 */
UCLASS(BlueprintType)
class DIGITALTWIN_API UCommunicationLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
};
