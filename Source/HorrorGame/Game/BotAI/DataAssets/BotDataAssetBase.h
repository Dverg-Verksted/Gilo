// It is owned by the company Dverg Verksted.

#pragma once

#include "CoreMinimal.h"
#include "Game/Common/AutoDiscoveryAsset.h"
#include "BotDataAssetBase.generated.h"

/**
 * Настройки бота
 */
UCLASS()
class HORRORGAME_API UBotDataAssetBase : public UAutoDiscoveryAsset
{
	GENERATED_BODY()

public:
	UBotDataAssetBase();

	/* Класс бота */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Common", DisplayName = "Класс бота", AssetRegistrySearchable,
		meta = (ToolTip = "Класс бота", AssetBundles = "classes", AllowedClasses = "BotBase"))
	TSoftClassPtr<AActor> BotClass;

	/* Скорость ходьбы бота */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Common", DisplayName = "Скорость ходьбы бота", meta = (ToolTip = "Скорость ходьбы бота"))
	float BotWalkSpeed = 320.0f;

protected:
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
};
