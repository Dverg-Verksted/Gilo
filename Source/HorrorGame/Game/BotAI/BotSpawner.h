// It is owned by the company Dverg Verksted.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DataRegistryId.h"
#include "BotSpawner.generated.h"

UCLASS(Abstract, Blueprintable, NotBlueprintType, ClassGroup = "AI", AutoExpandCategories = "Bot", HideCategories = (Rendering, HLOD, Collision, Cooking, Variable, Replication, Input, Actor))
class HORRORGAME_API ABotSpawner : public AActor
{
	GENERATED_BODY()

public:
	ABotSpawner(const FObjectInitializer& ObjectInitializer);

	/* Дата Ассет бота */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bot")
	FDataRegistryId BotID;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USceneComponent> SceneRoot;

	virtual void BeginPlay() override;
	void SpawnBot();
	void OnBotAssetLoaded(FPrimaryAssetId LoadedAssetID);
	/* Спаун и инициализация бота из дата ассета */
	UFUNCTION(BlueprintNativeEvent)
	void InitFromAsset(UPrimaryDataAsset* SourceAsset);
	/* Возвращает координаты спауна бота в WorldSpace */
	UFUNCTION(BlueprintNativeEvent)
	void GetSpawnTransform(FTransform& SpawnTransform);
};
