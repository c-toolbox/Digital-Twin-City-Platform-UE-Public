
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DigitalTwinPlayerController.generated.h"

/**
 *
 */
UCLASS()
class DIGITALTWIN_API ADigitalTwinPlayerController : public APlayerController {
	GENERATED_BODY()
public:
	ADigitalTwinPlayerController();
	virtual ~ADigitalTwinPlayerController() override;

	UFUNCTION()
	virtual void BeginPlay() override;
	UFUNCTION()
	virtual void SetupInputComponent() override;

};
