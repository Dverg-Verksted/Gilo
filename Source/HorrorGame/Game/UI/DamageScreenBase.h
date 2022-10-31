// It is owned by the company Dverg Verksted.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UObject/Object.h"
#include "DamageScreenBase.generated.h"

/**
 * Экран повреждений
 */
UCLASS(Abstract, Blueprintable)
class HORRORGAME_API UDamageScreenBase : public UUserWidget
{
	GENERATED_BODY()

protected:
	/* Время в течение которого виджет не реагирует на новые повреждения */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float DamageCooldown = 1.0f;

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	/* Событие добавления эффекта повреждения */
	UFUNCTION(BlueprintImplementableEvent)
	void OnAddDamageEffect();

	UFUNCTION()
	void OnPlayerPawnDamageHandler(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

private:
	/* TRUE - Если персонаж уже поврежден */
	bool bAlreadyDamaged = false;
	/* Таймер cooldown-а повреждения */
	FTimerHandle DamageCooldownTimer;

	/* Событие таймера завершения cooldown-а */
	UFUNCTION()
	void OnDamageCooldownHandler();
};
