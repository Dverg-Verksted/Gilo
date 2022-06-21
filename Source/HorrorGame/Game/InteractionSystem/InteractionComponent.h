// It is owned by the company Dverg Verksted.

#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "Components/ActorComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "InteractionComponent.generated.h"


/* Компонент для поиска и взаимодействия с интерактивными объектами */
UCLASS(BlueprintType, NotBlueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class HORRORGAME_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInteractionComponent();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInteractionObjectChangedEvent, AActor*, Actor, FHitResult, Hit);

	/* Интервал таймера трассировки */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Trace")
	float TraceTimerInterval = 0.1f;

	/* Дистанция трассировки */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Trace")
	float TraceDistance;

	/* Радиус сферы трассировки */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Trace")
	float TraceRadius;

	/* Дистанция удерживания объекта от игрока */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Trace")
	float GrabDistance = 100.0f;

	/* Типы объектов для трассировки */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Trace")
	TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjectTypes;

	/* Изменился текущий интерактивный объект */
	UPROPERTY(BlueprintAssignable)
	FOnInteractionObjectChangedEvent OnInteractionObjectChanged;

	/* Действие использования предмета */
	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> UseAction;

	/* Действие хватания предмета */
	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> GrabAction;

protected:
	UPROPERTY()
	TObjectPtr<APlayerController> PlayerController;

	UPROPERTY()
	TObjectPtr<APawn> PlayerPawn;

	/* Последний найденный интерактивный объект */
	UPROPERTY()
	TObjectPtr<AActor> LastInteractiveObject;

	UPROPERTY()
	TObjectPtr<UPhysicsHandleComponent> PhysicsHandleComponent;

	FTimerHandle TraceTimerHandle;

	/* TRUE - Трассировка включена */
	bool bTracingEnabled;

	/* TRUE - Если схватили и удерживаем объект */
	bool bGrabbingObject = false;

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/* Трассировка */
	void Trace(const FVector& Start, const FVector& Stop);

	/* Обработчик события таймера трассировки */
	UFUNCTION()
	void OnTraceTimerTick();

	/* Выбор нового интерактивного объекта */
	void SelectNewInteractionObject(const FHitResult& Hit);

	/* Очистка информации о последнем интерактивном объекте */
	void ClearLastInteractionObject();

	/** Обработчик действия использования предмета */
	UFUNCTION()
	void UseActionHandler(const FInputActionValue& ActionValue);

	/** Обработчик действия хватания предмета */
	UFUNCTION()
	void GrabActionHandler(const FInputActionValue& ActionValue);

	/** Обработчик завершения действия хватания предмета */
	UFUNCTION()
	void GrabActionStopHandler(const FInputActionValue& ActionValue);

public:
	/* Метод инициализации. Вызывается когда персонаж полностью готов к игре */
	void OnPlayerReady();

	/* Возвращает текущий интерактивный объект */
	FORCEINLINE AActor* GetCurrentInteractiveObject() const { return LastInteractiveObject.Get(); }

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/* Включение поиска интерактивных объектов */
	UFUNCTION(BlueprintCallable, Category="Trace")
	void StartTrace();

	/* Отключение поиска интерактивных объектов */
	UFUNCTION(BlueprintCallable, Category="Trace")
	void StopTrace();
};
