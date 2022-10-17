// It is owned by the company Dverg Verksted.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DataRegistryId.h"
#include "BotSpawner.generated.h"

/* Класс отвечает за спаун ботов */
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

	/* Спаун и инициализация бота из дата ассета */
	UFUNCTION(BlueprintNativeEvent, Category = "AI")
	void InitFromAsset(const UPrimaryDataAsset* SourceAsset);
	
	/* Возвращает координаты спауна бота в WorldSpace */
	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "AI")
	void GetSpawnTransform(FTransform& SpawnTransform);

	virtual void BeginPlay() override;
private:
	/* Спаун бота */
	void SpawnBot();
	/* Обработчик завершения загрузки дата ассета бота */
	void OnBotAssetLoaded(FPrimaryAssetId LoadedAssetID);
};
