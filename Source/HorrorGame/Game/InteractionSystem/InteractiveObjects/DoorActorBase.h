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
class HORRORGAME_API ADoorActorBase : public AActor, public IInteractiveObject, public IGameplayTagAssetInterface
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	FDataRegistryId DoorID;

	/* Минимальный угол открытия двери */
	UPROPERTY(EditDefaultsOnly, Category = "Door")
	float MinDoorAngle = 0.0f;

	/* Максимальный угол открытия двери */
	UPROPERTY(EditDefaultsOnly, Category = "Door")
	float MaxDoorAngle = 90.0f;

	/* Коэффициент открытия */
	UPROPERTY(EditDefaultsOnly, Category = "Door")
	float DragMagnitude = 10.0f;

	/* Скорость автоматического открытия */
	UPROPERTY(EditDefaultsOnly, Category = "Door")
	float AutoOpenSpeed = 5.0f;

	/* Действие открытия двери */
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> DoorDragAction;

	/* Продолжительное действие */
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> GrabObjectAction;

	/* Контекст действий с дверью */
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> DoorInputContext;

	/* Приоритет контекста управления дверью */
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	int32 DoorInputContextPriority = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameplayTags", Meta = (DisplayName = "GameplayTags", ExposeOnSpawn = true), SaveGame)
	FGameplayTagContainer GameplayTags;

protected:
	/* Требуемый угол открытия двери */
	float DesiredAngle = 0.0f;

	UPROPERTY()
	TObjectPtr<APlayerController> DragPlayerController;

	virtual void BeginPlay() override;
	void ReloadDoorAsset();

	UFUNCTION()
	void GrabObjectTriggeredHandler(const FInputActionValue& ActionValue);

	UFUNCTION()
	void GrabObjectCompletedHandler(const FInputActionValue& ActionValue);

	UFUNCTION()
	void DoorDragActionHandler(const FInputActionValue& ActionValue);

	UFUNCTION()
	void OnDoorAssetLoaded(FPrimaryAssetId LoadedAssetID);

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

public:
#pragma region InteractiveObjectInterface
	virtual void InitFromAsset_Implementation(UPrimaryDataAsset* SourceAsset) override;
	virtual void OnHoverBegin_Implementation(APlayerController* PlayerController, const FHitResult& Hit) override;
	virtual void OnHoverEnd_Implementation(APlayerController* PlayerController) override;
#pragma endregion

#pragma region GameplayTagAssetInterface
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override { TagContainer = GameplayTags; }
#pragma endregion

	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void Tick(float DeltaTime) override;
	virtual void OnUseObject_Implementation(APlayerController* PlayerController) override;
	/* Возвращает TRUE - Если дверь сейчас движется */
	FORCEINLINE bool IsMoving() const;

private:
	/* TRUE - Если уже привязались к вводу */
	bool bInputBinded = false;
	/* TRUE - Если двигаем дверь вручную (с помощью движений мыши/контроллера) */
	bool bDragged = false;
	/* TRUE - Если дверь дверь в процессе открытия/закрытия */
	bool bMoving = false;
	/* Угол поворота двери, с которого было начато движение */
	float MovingStartAngle = 0.0f;
	/* Требуемый угол поворота двери */
	float MovingTargetAngle = 0.0f;

	/* Начать движение двери к  */
	void StartMoving(float TargetAngle);
	/* Остановить автоматическое открытие/закрытие двери */
	void StopMoving();
	/* Вычисляет угол, максимально удаленный от текущего угла поворота двери */
	float CalculateOpenAngle() const;
};
