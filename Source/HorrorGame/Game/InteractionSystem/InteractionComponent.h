// It is owned by the company Dverg Verksted.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionComponent.generated.h"


/* Компонент для поиска и взаимодействия с интерактивными объектами */
UCLASS(BlueprintType, NotBlueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class HORRORGAME_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInteractionComponent();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInteractionObjectChangedEvent, AActor*, Actor, FHitResult, Hit);

	/* Дистанция трассировки */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Trace")
	float TraceDistance;

	/* Радиус сферы трассировки */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Trace")
	float TraceRadius;

	/* Типы объектов для трассировки */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Trace")
	TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjectTypes;

	/* Изменился текущий интерактивный объект */
	UPROPERTY(BlueprintAssignable)
	FOnInteractionObjectChangedEvent OnInteractionObjectChanged;
	
protected:
	UPROPERTY()
	TObjectPtr<APlayerController> PlayerController;

	UPROPERTY()
	TObjectPtr<APawn> PlayerPawn;

	/* Последний найденный интерактивный объект */
	UPROPERTY()
	TObjectPtr<AActor> LastInteractiveObject;

	/* TRUE - Трассировка включена */
	bool bTracingEnabled;

	virtual void BeginPlay() override;
	/* Трассировка */
	void Trace(const FVector& Start, const FVector& Stop);

	/* Выбор нового интерактивного объекта */
	void SelectNewInteractionObject(const FHitResult& Hit);

	/* Очистка информации о последнем интерактивном объекте */
	void ClearLastInteractionObject();

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/* Включение поиска интерактивных объектов */
	UFUNCTION(BlueprintCallable, Category="Trace")
	void StartTrace();

	/* Отключение поиска интерактивных объектов */
	UFUNCTION(BlueprintCallable, Category="Trace")
	void StopTrace();
};
