
#pragma once
#include "Metadata.h"

#include "Dataset.h"
#include "CoreMinimal.h"

#include "MediaDataset.generated.h"

/**
* UMediaDataset
* This is not the scene representation of a Movie
*/

UCLASS()
class DIGITALTWIN_API UMediaDataset : public UDataset
{
  GENERATED_BODY()
public:
  UMediaDataset();
  virtual void ActivateDataset(UWorld* World,int32 SortOrder, FString MaterialPath) override;
  virtual void ToggleDataset(UWorld* World) override;
  virtual void DisableDataset(UWorld* World) override;


  void SetTexture(class UMediaTexture* InTexture);

  const FMediaDataRow& GetMetaData() const
  {
    return MetaData;
  }

  void SetMetaData(const FMediaDataRow& InMetadata);

  //Gets the currently held MediaTexture. 
  //Might return nullptr if no Texture is being held
  UFUNCTION()
    class UMediaTexture* GetMediaTexture() const;

  //This needs to be "BlueprintCallable" for Latent actions to work.
  UFUNCTION(BlueprintCallable)
    void PlaybackResumed();
private:
  UPROPERTY()
    class UMaterial* MediaDecalMaterial;

  UPROPERTY()
    class UMediaTexture* MediaTexture;

  UPROPERTY()
    class ARasterActor* RasterActor;

  FMediaDataRow MetaData;
};

