// It is owned by the company Dverg Verksted.

#include "Game/Player/WalkCameraShakeComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UWalkCameraShakeComponent::UWalkCameraShakeComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UWalkCameraShakeComponent::BeginPlay()
{
	Super::BeginPlay();
	StartCameraShakes();
}

void UWalkCameraShakeComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	StopCameraShakes();
}

void UWalkCameraShakeComponent::ToggleMoveAction(bool bMoveActionEnabled)
{
	bMoveInputActive = bMoveActionEnabled;
}

void UWalkCameraShakeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!IsActive()) return;

	bool bFalling = false;
	const auto* Pawn = Cast<ACharacter>(GetOwner());
	if (Pawn && Pawn->GetCharacterMovement()->IsFalling())
	{
		bFalling = true;
	}

	const bool bMoving = bMoveInputActive;
	if (!bMoving && !bFalling)
	{
		CameraMoveState = ECameraMoveState::Idle;
	}
	else if (bMoving && !bSprinting && CameraMoveState != ECameraMoveState::Walk)
	{
		CameraMoveState = ECameraMoveState::Walk;
	}
	else if (bMoving && bSprinting && CameraMoveState != ECameraMoveState::Run)
	{
		CameraMoveState = ECameraMoveState::Run;
	}

	// Сглаживание переходов между CameraShake
	for (const auto& Pair : CameraShakes)
	{
		if (Pair.Value && Pair.Key != CameraMoveState)
		{
			const float NewScale = FMath::FInterpConstantTo(Pair.Value->ShakeScale, 0.0f, DeltaTime, ShakeOutSpeed);
			if (Pair.Value->ShakeScale > 0.0f)
			{
				Pair.Value->ShakeScale = NewScale;
			}
		}
		else
		{
			Pair.Value->ShakeScale = FMath::FInterpConstantTo(Pair.Value->ShakeScale, 1.0f, DeltaTime, ShakeInSpeed);
		}
	}
}

void UWalkCameraShakeComponent::StartCameraShakes()
{
	const APlayerController* PC = GetPlayerController();
	ensure(PC);
	if (!PC) return;

	UCameraShakeBase* Shake = nullptr;
	if (IdleCameraShakeClass)
	{
		Shake = PC->PlayerCameraManager->StartCameraShake(IdleCameraShakeClass, 0.1f);
		CameraShakes.Add(ECameraMoveState::Idle, Shake);
	}
	if (WalkCameraShakeClass)
	{
		Shake = PC->PlayerCameraManager->StartCameraShake(WalkCameraShakeClass, 0.1f);
		CameraShakes.Add(ECameraMoveState::Walk, Shake);
	}
	if (RunCameraShakeClass)
	{
		Shake = PC->PlayerCameraManager->StartCameraShake(RunCameraShakeClass, 0.1f);
		CameraShakes.Add(ECameraMoveState::Run, Shake);
	}
}

void UWalkCameraShakeComponent::StopCameraShakes()
{
	if (const APlayerController* PC = GetPlayerController())
	{
		for (const auto& Pair : CameraShakes)
		{
			PC->PlayerCameraManager->StopCameraShake(Pair.Value, true);
		}
	}
	CameraShakes.Empty();
}

APlayerController* UWalkCameraShakeComponent::GetPlayerController() const
{
	if (const APawn* OwnerPawn = Cast<APawn>(GetOwner()))
	{
		if (auto* PC = Cast<APlayerController>(OwnerPawn->GetController()))
		{
			return PC;
		}
	}
	UE_LOG(LogTemp, Error, TEXT("WalkCameraShakeComponent не может найти PlayerController"));
	return nullptr;
}

void UWalkCameraShakeComponent::ToggleSprinting(bool bEnabled)
{
	bSprinting = bEnabled;
}
