// It is owned by the company Dverg Verksted.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DataRegistryId.h"
#include "BotSpawner.generated.h"

UCLASS()
class HORRORGAME_API ABotSpawner : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABotSpawner();

	/* Дата Ассет бота */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bot")
	FDataRegistryId BotID;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void SpawnBot();
	void OnBotAssetLoaded(FPrimaryAssetId LoadedAssetID);
	UFUNCTION(BlueprintNativeEvent)
	void InitFromAsset(UPrimaryDataAsset* SourceAsset);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
