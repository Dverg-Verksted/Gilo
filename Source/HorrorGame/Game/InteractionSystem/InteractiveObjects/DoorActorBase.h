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

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuickOpenCloseCompleteHandler, float, TargetAngle);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDoorDragHandler, float, Speed, float, Progress);

	/* Событие вызывается по завершении автоматического открытия/закрытия двери */
	UPROPERTY(BlueprintAssignable)
	FOnQuickOpenCloseCompleteHandler OnQuickOpenCloseComplete;

	/* Событие вызывается в процессе ручного открытия двери */
	UPROPERTY(BlueprintAssignable)
	FOnDoorDragHandler OnDoorDrag;

	/* Дата Ассет текущей двери */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InteractiveObjects|Door")
	FDataRegistryId DoorID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameplayTags", Meta = (DisplayName = "GameplayTags", ExposeOnSpawn = true), SaveGame)
	FGameplayTagContainer GameplayTags;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USceneComponent> DoorRootComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> DoorMeshComponent;

	/* Угол поворота открытой двери в автоматическом режиме */
	UPROPERTY(EditDefaultsOnly, Category = "InteractiveObjects|Door")
	float DoorOpenedAngle = 90.0f;

	/* Угол поворота закрытой двери в автоматическом режиме */
	UPROPERTY(EditDefaultsOnly, Category = "InteractiveObjects|Door")
	float DoorClosedAngle = 0.0f;

	/* Минимальный угол открытия двери */
	UPROPERTY(EditDefaultsOnly, Category = "InteractiveObjects|Door")
	float MinDoorAngle = 0.0f;

	/* Максимальный угол открытия двери */
	UPROPERTY(EditDefaultsOnly, Category = "InteractiveObjects|Door")
	float MaxDoorAngle = 90.0f;

	/* Коэффициент открытия */
	UPROPERTY(EditDefaultsOnly, Category = "InteractiveObjects|Door")
	float DragMagnitude = 10.0f;

	/* Скорость автоматического открытия */
	UPROPERTY(EditDefaultsOnly, Category = "InteractiveObjects|Door")
	float AutoOpenSpeed = 5.0f;

	/* Действие открытия двери */
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> DoorDragAction;

	/* Продолжительное действие */
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> GrabObjectAction;

	/* Действие быстрого открытия/закрытия двери */
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> QuickOpenCloseAction;

	/* Контекст действий с дверью */
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> DoorInputContext;

	/* Приоритет контекста управления дверью */
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	int32 DoorInputContextPriority = 10;

	/* Требуемый угол открытия двери */
	float DesiredAngle = 0.0f;
	/* TRUE - Если разрешается открытие с помощью движения мыши/контроллера */
	bool bDragEnabled = true;

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
	void QuickOpenCloseActionHandler(const FInputActionValue& InputActionValue);

	UFUNCTION()
	void OnDoorAssetLoaded(FPrimaryAssetId LoadedAssetID);

	/* Событие вызывается при начале быстрого открытия двери */
	UFUNCTION(BlueprintImplementableEvent)
	void OnDoorQuickOpen();

	/* Событие вызывается при начале быстрого закрытия двери */
	UFUNCTION(BlueprintImplementableEvent)
	void OnDoorQuickClose();

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

public:
#pragma region GettersSetters
	/* Возвращает TRUE - Если возможно ручное открытие */
	UFUNCTION(BlueprintPure, Category = "InteractiveObjects|Door")
	FORCEINLINE bool IsDragEnabled() const { return bDragEnabled; }
#pragma endregion

#pragma region InteractiveObjectInterface
	virtual void InitFromAsset_Implementation(UPrimaryDataAsset* SourceAsset) override;
	virtual void OnHoverBegin_Implementation(APlayerController* PlayerController, const FHitResult& Hit) override;
	virtual void OnHoverEnd_Implementation(APlayerController* PlayerController) override;
#pragma endregion

#pragma region GameplayTagAssetInterface
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override { TagContainer = GameplayTags; }
#pragma endregion
	/* Открыть дверь */
	UFUNCTION(BlueprintCallable, Category = "InteractiveObjects|Door")
	void OpenDoor();
	/* Закрыть дверь */
	UFUNCTION(BlueprintCallable, Category = "InteractiveObjects|Door")
	void CloseDoor();

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
	float CalculateOpenAngle(bool& bOpen) const;
	void QuickToggleDoor();
};
