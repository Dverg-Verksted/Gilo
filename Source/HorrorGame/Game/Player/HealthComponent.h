// It is owned by the company Dverg Verksted.

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
	/* Здоровье игрока */
	UPROPERTY()
	float Health = 100.0f;

	/* Макс. Здоровье игрока */
	UPROPERTY()
	float MaxHealth = 100.0f;

	/* Задает здоровье игрока */
	UFUNCTION(BlueprintCallable)
	void SetHealth(float InHealth);

	UFUNCTION()
	virtual void OnOwnerTakeDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	/* Обновляет здоровье игрока на заданное значение */
	virtual void UpdateHealth(float InAmount);

public:
	/* Возвращает здоровье игрока */
	UFUNCTION(BlueprintPure, Category = "Player")
	float GetPlayerHealth() const { return Health; };

	/* Возвращает макс. здоровье игрока */
	UFUNCTION(BlueprintPure, Category = "Player")
	float GetPlayerMaxHealth() const { return MaxHealth; };
};
