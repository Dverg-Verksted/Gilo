// It is owned by the company Dverg Verksted.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnhancedInputComponent.h"
#include "GameplayTagAssetInterface.h"
#include "PlayerSprintComponent.h"
#include "WalkCameraShakeComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/TimelineComponent.h"
#include "Game/InteractionSystem/InteractionComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UWalkCameraShakeComponent> WalkCameraShakeComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UInteractionComponent> InteractionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UPhysicsHandleComponent> PhysicsHandleComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameplayTags", Meta = (DisplayName = "GameplayTags", ExposeOnSpawn = true), SaveGame)
	FGameplayTagContainer GameplayTags;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	FVector PeekLeftOffset;

	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	FVector PeekRightOffset;

	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	float PeekRotation;

	UPROPERTY(EditDefaultsOnly, Category = "Input", AdvancedDisplay)
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input", AdvancedDisplay)
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input", AdvancedDisplay)
	TObjectPtr<UInputAction> PeekAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input", AdvancedDisplay)
	TObjectPtr<UInputAction> CrouchAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input", AdvancedDisplay)
	TObjectPtr<UInputAction> SprintAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input", meta = (ToolTip = "Контекст ввода по-умолчанию"), AdvancedDisplay)
	TObjectPtr<UInputMappingContext> DefaultInputContext;

	/** Макс. величина замедления перемещения при наклоне */
	UPROPERTY()
	float MaxPeekSlowDown;

	FTimeline PeekTimeline;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UCurveFloat> PeekCurve;

	/* Текущее состояние наклона вбок */
	EPlayerPeekState PeekState;

	/* Модификатор скорости движения влево-вправо */
	UPROPERTY()
	float StrafeMoveMagnitude;

	/* Модификатор скорости движения назад */
	UPROPERTY()
	float BackMoveMagnitude;

	UPROPERTY()
	APlayerController* PlayerController;

	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;
	virtual void PawnClientRestart() override;
	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override { TagContainer = GameplayTags; }

private:
	/* Трансформ камеры по-умолчанию */
	FTransform CameraDefaultTransform;

	FVectorSpringState CameraInterpSpringState;

	/** TRUE - Если нажаты клавиши движения */
	bool bMoveInputActive = false;

	/** Текущая альфа наклона От 0 до 1 */
	float PeekAlpha;

	/* Текущее значение смещения камеры в приседе */
	FVector CameraCrouchOffset;

	/* Текущее значение смещения камеры в наклоне */
	FVector CameraPeekOffset;

	/** Обработчик начала ходьбы */
	UFUNCTION()
	void MoveStartActionHandler(const FInputActionValue& ActionValue);

	/** Обработчик ввода ходьбы */
	UFUNCTION()
	void MoveActionHandler(const FInputActionValue& ActionValue);

	/** Обработчик завершения ходьбы */
	UFUNCTION()
	void MoveStopActionHandler(const FInputActionValue& ActionValue);

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
