// It is owned by the company Dverg Verksted.


#include "Game/Player/WalkCameraShakeComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"


UWalkCameraShakeComponent::UWalkCameraShakeComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	CurrentCameraShake = nullptr;
}


void UWalkCameraShakeComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UWalkCameraShakeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!IsActive())
		return;

	if (const auto* Pawn = Cast<ACharacter>(GetOwner()))
	{
		if (Pawn->GetCharacterMovement()->IsFalling())
		{
			if (bCameraShakeActive)
			{
				StopCameraShake();
			}
			return;
		}
	}

	const FVector Velocity = GetOwner()->GetVelocity();
	const bool bMoving = Velocity.Size() != 0.f;

	if (!bMoving && (CameraMoveState != Idle || !bCameraShakeActive))
	{
		CameraMoveState = Idle;
		StartCameraShake(IdleCameraShakeClass);
	}
	else if (bMoving && !bSprinting && CameraMoveState != Walk)
	{
		CameraMoveState = Walk;
		StartCameraShake(WalkCameraShakeClass);
	}
	else if (bMoving && bSprinting && CameraMoveState != Run)
	{
		CameraMoveState = Run;
		StartCameraShake(RunCameraShakeClass);
	}

	// Сглаживание переходов между CameraShake
	if (OldCameraShake)
	{
		const float NewScale = FMath::FInterpConstantTo(OldCameraShake->ShakeScale, 0.0f, DeltaTime, ShakeOutSpeed);
		if (OldCameraShake->ShakeScale > 0.01f)
		{
			OldCameraShake->ShakeScale = NewScale;
		}
		else
		{
			OldCameraShake->StopShake(true);
			OldCameraShake = nullptr;
		}
	}
	if (CurrentCameraShake)
	{
		if (CurrentCameraShake->ShakeScale < 1.0f)
		{
			const float NewScale = FMath::FInterpConstantTo(CurrentCameraShake->ShakeScale, 1.0f, DeltaTime, ShakeInSpeed);
			CurrentCameraShake->ShakeScale = NewScale;
		}
	}
}

void UWalkCameraShakeComponent::StartCameraShake(TSubclassOf<UCameraShakeBase> CameraShakeClass)
{
	const APlayerController* PC = GetPlayerController();
	ensure(PC);

	StopCameraShake();

	if (!CameraShakeClass)
	{
		UE_LOG(LogTemp, Error, TEXT("Не задан класс CameraShake"));
		return;
	}

	CurrentCameraShake = PC->PlayerCameraManager->StartCameraShake(CameraShakeClass, 0.1f);
	bCameraShakeActive = true;
}

void UWalkCameraShakeComponent::StopCameraShake()
{
	const APlayerController* PC = GetPlayerController();
	ensure(PC);

	if (bCameraShakeActive)
	{
		bCameraShakeActive = false;
		OldCameraShake = CurrentCameraShake;
	}
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
