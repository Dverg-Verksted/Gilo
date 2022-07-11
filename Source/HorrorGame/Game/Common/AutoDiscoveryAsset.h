// It is owned by the company Dverg Verksted.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AutoDiscoveryAsset.generated.h"

UCLASS()
class HORRORGAME_API UAutoDiscoveryAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	/* Уникальное имя ассета */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = "Asset", DisplayName = "Уникальное имя", meta = (ToolTip = "Имя ассета должно быть уникальным"))
	FName AssetName;

	virtual void GetAssetRegistryTags(TArray<FAssetRegistryTag>& OutTags) const override;

protected:
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
