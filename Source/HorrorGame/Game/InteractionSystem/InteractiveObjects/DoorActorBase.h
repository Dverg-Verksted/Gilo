// It is owned by the company Dverg Verksted.

#pragma once

#include "CoreMinimal.h"
#include "DataRegistryId.h"
#include "EnhancedInputComponent.h"
#include "GameplayTagAssetInterface.h"
#include "InputAction.h"
#include "Game/InteractionSystem/InteractiveObject.h"
#include "GameFramework/Actor.h"
#include "DoorActorBase.generated.h"

/* Базовый класс для дверей */
UCLASS()
class HORRORGAME_API ADoorActorBase
	: public AActor,
	  public IInteractiveObject,
	  public IGameplayTagAssetInterface
{
	GENERATED_BODY()

public:
	ADoorActorBase(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USceneComponent> DoorRootComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> DoorMeshComponent;
	
	/* Дата Ассет текущей двери */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Door")
	FDataRegistryId DoorID;

	/* Минимальный угол открытия двери */
	UPROPERTY(EditDefaultsOnly, Category="Door")
	float MinDoorAngle = 0.0f;

	/* Максимальный угол открытия двери */
	UPROPERTY(EditDefaultsOnly, Category="Door")
	float MaxDoorAngle = 90.0f;

	/* Коэффициент открытия */
	UPROPERTY(EditDefaultsOnly, Category="Door")
	float DragMagnitude = 10.0f;


	/* Действие открытия двери */
	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> DoorDragAction;

	/* Контекст действий с дверью */
	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputMappingContext> DoorInputContext;

	/* Приоритет контекста управления дверью */
	UPROPERTY(EditDefaultsOnly, Category="Input")
	int32 DoorInputContextPriority = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameplayTags",
		Meta = (DisplayName = "GameplayTags", ExposeOnSpawn = true), SaveGame)
	FGameplayTagContainer GameplayTags;

protected:
	/* Требуемый угол открытия двери */
	float DesiredAngle = 0.0f;

	UPROPERTY()
	TObjectPtr<APlayerController> DragPlayerController;

	virtual void BeginPlay() override;
	void ReloadDoorAsset();

	UFUNCTION()
	void DoorDragActionHandler(const FInputActionValue& ActionValue);

	UFUNCTION()
	void DoorDragStopActionHandler(const FInputActionValue& ActionValue);

	UFUNCTION()
	void OnDoorAssetLoaded(FPrimaryAssetId LoadedAssetID);

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

public:
	virtual void OnConstruction(const FTransform& Transform) override;

	// Interactive object interface BEGIN
	virtual void InitFromAsset_Implementation(UPrimaryDataAsset* SourceAsset) override;
	virtual void OnHoverBegin_Implementation(APlayerController* PlayerController, const FHitResult& Hit) override;
	virtual void OnHoverEnd_Implementation(APlayerController* PlayerController) override;
	// Interactive object interface END

	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override
	{
		TagContainer = GameplayTags;
	}

	virtual void Tick(float DeltaTime) override;
private:
	/* TRUE - Если уже привязались к вводу */
	bool bInputBinded = false;
	/* TRUE - Если двигаем дверь */
	bool bDragged = false;
};
