﻿// It is owned by the company Dverg Verksted.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class HORRORGAME_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHealthComponent();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeathDelegate);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthChangedDelegate, float, HealthNormalized);

	/* Событие вызывается когда обладатель здоровья умирает */
	UPROPERTY(BlueprintAssignable)
	FOnDeathDelegate OnDeath;

	/* Событие вызывается когда меняется здоровье обладателя */
	UPROPERTY(BlueprintAssignable)
	FOnHealthChangedDelegate OnHealthChanged;

protected:
	/* Текущий уровень здоровья */
	UPROPERTY()
	float Health = 100.0f;

	/* Макс. запас здоровья */
	UPROPERTY()
	float MaxHealth = 100.0f;

	/* Время от урона до полного восстановления здоровья */
	UPROPERTY()
	float HealthRegenDelay = 7.0f;

	/* Изменение значения здоровья */
	UFUNCTION(BlueprintCallable)
	void SetHealth(float InHealth);

	/* TRUE - Если обладатель жив */
	bool bAlive = true;

	/* Обработчик события урона владельцу компонента */
	UFUNCTION()
	virtual void OnOwnerTakeDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	/* Обновляет здоровье на заданное значение */
	virtual void UpdateHealth(float InAmount);

public:
	/* Возвращает текущее здоровье */
	UFUNCTION(BlueprintPure, Category = "Health")
	float GetHealth() const { return Health; };

	/* Возвращает макс. уровень здоровья */
	UFUNCTION(BlueprintPure, Category = "Health")
	float GetMaxHealth() const { return MaxHealth; };

	/* Возвращает TRUE - Если обладатель жив */
	UFUNCTION(BlueprintPure, Category = "Health")
	bool IsAlive() const { return bAlive; };

	/* Инициализация компонента */
	UFUNCTION(BlueprintCallable, Category = "Health")
	void Initialize(const float& InMaxHealth, const float& InHealthRegenDelay);

private:
	/* Таймер восстановления здоровья */
	FTimerHandle RegenTimerHandle;
	/* Отвязка от всех событий */
	void ClearEvents();
	/* Остановка и очистка таймера регенерации здоровья */
	void ClearHealthRegenTimer();
};
