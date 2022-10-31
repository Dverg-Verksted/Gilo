// It is owned by the company Dverg Verksted.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WalkCameraShakeComponent.generated.h"

/* Состояние камеры зависящее от текущего движения персонажа */
UENUM()
enum class ECameraMoveState : uint8
{
	NoShake,
	Idle,
	Walk,
	Run
};

/* Компонент отвечает за раскачивание камеры игрока в покое, при ходьбе и беге */
UCLASS(BlueprintType, NotBlueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class HORRORGAME_API UWalkCameraShakeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UWalkCameraShakeComponent();

	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	TSubclassOf<UCameraShakeBase> IdleCameraShakeClass;

	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	TSubclassOf<UCameraShakeBase> WalkCameraShakeClass;

	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	TSubclassOf<UCameraShakeBase> RunCameraShakeClass;

	/* Скорость плавного начала нового CameraShake */
	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	float ShakeInSpeed = 5.0f;

	/* Скорость плавного завершения старого CameraShake */
	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	float ShakeOutSpeed = 2.5f;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** TRUE - Если поступают команды на движение */
	bool bMoveInputActive = false;

	UPROPERTY()
	TMap<ECameraMoveState, UCameraShakeBase*> CameraShakes;

	/* Признак бежим или нет */
	bool bSprinting = false;

	/* Текущий тип движения камеры */
	ECameraMoveState CameraMoveState = ECameraMoveState::NoShake;

	/* Возвращает текущий PlayerController */
	APlayerController* GetPlayerController() const;

public:
	UFUNCTION(BlueprintCallable, Category = "WalkCameraShake")
	void ToggleSprinting(bool bEnabled);

	UFUNCTION(BlueprintCallable, Category = "WalkCameraShake")
	void ToggleMoveAction(bool bMoveActionEnabled);

	/* Возвращает текущее состояние */
	FORCEINLINE ECameraMoveState GetState() const { return CameraMoveState; }

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void StartCameraShakes();
	void StopCameraShakes();
};
