// It is owned by the company Dverg Verksted.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnhancedInputComponent.h"
#include "GameplayTagAssetInterface.h"
#include "PlayerSprintComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "PlayerCharacterBase.generated.h"

/** Состояние наклона персонажа */
UENUM()
enum EPlayerPeekState
{
	Default,
	PeekLeft,
	PeekRight,
};

UCLASS()
class HORRORGAME_API APlayerCharacterBase : public ACharacter, public IGameplayTagAssetInterface
{
	GENERATED_BODY()

public:
	APlayerCharacterBase(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UCameraComponent> MainCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USpringArmComponent> MainCameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UPlayerSprintComponent> PlayerSprintComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameplayTags",
		Meta = (DisplayName = "GameplayTags", ExposeOnSpawn = true), SaveGame)
	FGameplayTagContainer GameplayTags;

protected:
	UPROPERTY(EditDefaultsOnly, Category="Camera")
	FTransform CameraDefaultTransform;

	UPROPERTY(EditDefaultsOnly, Category="Camera")
	FVector PeekLeftOffset;

	UPROPERTY(EditDefaultsOnly, Category="Camera")
	FVector PeekRightOffset;

	UPROPERTY(EditDefaultsOnly, Category="Camera")
	float PeekRotation;

	UPROPERTY(EditDefaultsOnly, Category="Input", AdvancedDisplay)
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, Category="Input", AdvancedDisplay)
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditDefaultsOnly, Category="Input", AdvancedDisplay)
	TObjectPtr<UInputAction> PeekAction;

	UPROPERTY(EditDefaultsOnly, Category="Input", AdvancedDisplay)
	TObjectPtr<UInputAction> CrouchAction;

	UPROPERTY(EditDefaultsOnly, Category="Input", AdvancedDisplay)
	TObjectPtr<UInputAction> SprintAction;

	UPROPERTY(EditDefaultsOnly, Category="Input", meta=(ToolTip="Контекст ввода по-умолчанию"),
		AdvancedDisplay)
	TObjectPtr<UInputMappingContext> DefaultInputContext;

	/** Макс. величина замедления перемещения при наклоне */
	UPROPERTY(EditDefaultsOnly, Category="Input",
		meta=(ToolTip="Макс. величина замедления перемещения при наклоне", ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"
		), AdvancedDisplay)
	float MaxPeekSlowDown;

	FTimeline PeekTimeline;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UCurveFloat> PeekCurve;

	EPlayerPeekState PeekState;

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void BeginPlay() override;

	virtual void PawnClientRestart() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override
	{
		TagContainer = GameplayTags;
	}

private:
	/** Текущая альфа наклона От 0 до 1 */
	float PeekAlpha;

	/** Обработчик ввода ходьбы */
	UFUNCTION()
	void MoveActionHandler(const FInputActionValue& ActionValue);

	/** Обработчик ввода поворота камеры */
	UFUNCTION()
	void LookActionHandler(const FInputActionValue& ActionValue);

	/** Обработчик ввода наклона */
	UFUNCTION()
	void PeekActionHandler(const FInputActionValue& ActionValue);

	/** Обработчик завершения наклона */
	UFUNCTION()
	void PeekStopHandler(const FInputActionValue& ActionValue);

	/** Обработчик начала бега */
	UFUNCTION()
	void SprintStartHandler(const FInputActionValue& ActionValue);

	/** Обработчик остановки бега */
	UFUNCTION()
	void SprintStopHandler(const FInputActionValue& ActionValue);

	UFUNCTION()
	void PeekTimelineProgress(float Value);

	UFUNCTION()
	void PeekTimelineFinished();

	/** Обработчик ввода приседа */
	UFUNCTION()
	void CrouchActionHandler(const FInputActionValue& ActionValue);
};
