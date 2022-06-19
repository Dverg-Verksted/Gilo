// It is owned by the company Dverg Verksted.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PlayerSprintComponent.generated.h"


UCLASS(BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class HORRORGAME_API UPlayerSprintComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPlayerSprintComponent();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStaminaChangedEvent, float, Stamina);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnExhaustedChangedEvent, bool, Exhausted, float, Stamina);

	/** Скорость ходьбы */
	UPROPERTY(EditDefaultsOnly, Category="Movement")
	float WalkSpeed;

	/** Скорость бега */
	UPROPERTY(EditDefaultsOnly, Category="Movement")
	float RunSpeed;

	/** Мин. количество выносливости для возобновления бега */
	UPROPERTY(EditDefaultsOnly, Category="Movement")
	float MinStaminaAmount;

	/** Темп снижения выносливости */
	UPROPERTY(EditDefaultsOnly, Category="Movement")
	float StaminaDrainRate;

	/** Темп восстановления выносливости */
	UPROPERTY(EditDefaultsOnly, Category="Movement")
	float StaminaRestoreRate;

	/** Вызывается при обновлении текущего значения выносливости */
	UPROPERTY(BlueprintAssignable)
	FOnStaminaChangedEvent OnStaminaChanged;

	/** Вызывается при изменения флага выматанности */
	UPROPERTY(BlueprintAssignable)
	FOnExhaustedChangedEvent OnExhaustedChanged;

	/** Текущий запас выносливости, от 0 до 1*/
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

	virtual void BeginPlay() override;

public:
	/** Переключение режима бега */
	UFUNCTION()
	void ToggleSprint(bool bSprintEnabled);

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
