// It is owned by the company Dverg Verksted.

#pragma once

#include "CoreMinimal.h"
#include "Game/Common/AutoDiscoveryAsset.h"
#include "DrawerDataAsset.generated.h"

/**
 * Выдвижной ящик
 */
UCLASS()
class HORRORGAME_API UDrawerDataAsset : public UAutoDiscoveryAsset
{
	GENERATED_BODY()

public:
	UDrawerDataAsset();

	/* Модель ящика */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drawer", DisplayName="Модель", AssetRegistrySearchable,
		meta=(ToolTip="Модель ящика", AssetBundles="meshes"))
	TSoftObjectPtr<UStaticMesh> DrawerMesh;

	/* Максимальная дистанция выдвижения ящика */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drawer", DisplayName="Макс. дистанция", meta=(ToolTip="Макс. дистанция выдвижения ящика"))
	float MaxExtendDistance = 45.0f;

	/* Тяжесть открытия */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drawer", DisplayName="Тяжесть открытия",
		meta=(ToolTip="Тяжесть открытия. Чем меньше - тем тяжелее"))
	float DragMagnitude = 10.0f;

protected:
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
};
