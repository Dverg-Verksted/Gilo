﻿// It is owned by the company Dverg Verksted.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PlayerSprintComponent.generated.h"

UCLASS(BlueprintType, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class HORRORGAME_API UPlayerSprintComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPlayerSprintComponent();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStaminaChangedEvent, float, Stamina, float, Fatigue);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnExhaustedChangedEvent, bool, Exhausted, float, Stamina);

	/** Скорость ходьбы */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float WalkSpeed;

	/* Макс. скорость ходьбы в приседе */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float MaxWalkSpeedCrouched;

	/** Скорость бега */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float RunSpeedMagnitude;

	/** Макс. количество выносливости*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float MaxStaminaAmount;

	/** Мин. количество выносливости для возобновления бега */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float MinStaminaAmount;

	/** Темп снижения выносливости */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float StaminaDrainRate;

	/** Темп восстановления выносливости */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float StaminaRestoreRate;

	/** Вызывается при обновлении текущего значения выносливости */
	UPROPERTY(BlueprintAssignable)
	FOnStaminaChangedEvent OnStaminaChanged;

	/** Вызывается при изменения флага выматанности */
	UPROPERTY(BlueprintAssignable)
	FOnExhaustedChangedEvent OnExhaustedChanged;

	/** Текущий запас выносливости */
	UPROPERTY()
	float CurrentStamina;

protected:
	UPROPERTY()
	TObjectPtr<ACharacter> OwnerCharacter;

	UPROPERTY()
	TObjectPtr<UCharacterMovementComponent> CharacterMovement;

	/** TRUE - Если в режиме бега */
	UPROPERTY()
	bool bSprinting;

	/** TRUE - Если персонаж устал и не может бежать */
	UPROPERTY()
	bool bExhausted;

	/** TRUE - Если поступают команды на движение */
	UPROPERTY()
	bool bMoveInputActive;

	virtual void BeginPlay() override;

public:
	/* Возвращает TRUE - Если бежим */
	FORCEINLINE bool IsSprinting() const { return bSprinting; }

	/** Переключение активности команд движения */
	UFUNCTION()
	void ToggleMoveActionInput(bool bMoveActionEnabled);

	/** Переключение режима бега */
	UFUNCTION()
	void ToggleSprint(bool bSprintEnabled);

	/* Обновление значений выносливости */
	void UpdateStamina(float DeltaTime);

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
