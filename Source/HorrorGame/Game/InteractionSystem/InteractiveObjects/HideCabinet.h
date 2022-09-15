// It is owned by the company Dverg Verksted.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagAssetInterface.h"
#include "Game/InteractionSystem/InteractiveObject.h"
#include "GameFramework/Actor.h"
#include "HideCabinet.generated.h"

UCLASS()
class HORRORGAME_API AHideCabinet : public AActor, public IInteractiveObject, public IGameplayTagAssetInterface
{
	GENERATED_BODY()

public:
	AHideCabinet(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameplayTags", Meta = (DisplayName = "GameplayTags", ExposeOnSpawn = true), SaveGame)
	FGameplayTagContainer GameplayTags;

protected:
	/* Событие вызывается когда игрок прячется в шкафу */
	UFUNCTION(BlueprintImplementableEvent)
	void OnPlayerEnter(APlayerController* PlayerController);

	/* Событие вызывается когда игрок выходит из шкафа */
	UFUNCTION(BlueprintImplementableEvent)
	void OnPlayerExit(APlayerController* PlayerController);

	/* TRUE - Если шкаф занят */
	bool bOccupied = false;

	virtual void BeginPlay() override;
	virtual void OnUseObject_Implementation(APlayerController* PlayerController) override;
	/* Попытка занять шкаф пользователем */
	void OccupyCabinet(APlayerController* PlayerController);
	/* Попытка освободить шкаф пользователем */
	void ReleaseCabinet(APlayerController* PlayerController);
	/* Открывает все двери шкафа */
	UFUNCTION(BlueprintCallable)
	void OpenAllDoors();
	/* Закрывает все двери шкафа */
	UFUNCTION(BlueprintCallable)
	void CloseAllDoors();
	/* Возвращает коллекцию дверей шкафа */
	void GetDoors(TArray<class ADoorActorBase*>& Doors) const;

public:
#pragma region GameplayTagAssetInterface
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override { TagContainer = GameplayTags; }
#pragma endregion
};
