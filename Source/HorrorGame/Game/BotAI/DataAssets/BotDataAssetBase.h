// It is owned by the company Dverg Verksted.

#pragma once

#include "CoreMinimal.h"
#include "Game/Common/AutoDiscoveryAsset.h"
#include "BotDataAssetBase.generated.h"

/**
 * Бот
 */
UCLASS()
class HORRORGAME_API UBotDataAssetBase : public UAutoDiscoveryAsset
{
	GENERATED_BODY()

public:
	UBotDataAssetBase();

	/* Класс бота */
	UPROPERTY(
		EditDefaultsOnly, BlueprintReadOnly, Category = "Door", DisplayName = "Класс бота", AssetRegistrySearchable, meta = (ToolTip = "Класс бота", AssetBundles = "classes", MetaClass = "BotBase"))
	TSoftClassPtr<AActor> BotClass;

protected:
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
};
