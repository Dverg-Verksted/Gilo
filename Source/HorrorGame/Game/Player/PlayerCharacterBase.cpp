// It is owned by the company Dverg Verksted.

#include "Game/Player/PlayerCharacterBase.h"
#include "EnhancedInputSubsystems.h"
#include "PlayerSettings.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

APlayerCharacterBase::APlayerCharacterBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	StrafeMoveMagnitude = 0.75f;
	BackMoveMagnitude = 0.6f;
	MaxPeekSlowDown = 0.3f;
	PeekAlpha = 0.0f;
	PeekState = Default;

	PeekLeftOffset = FVector(0.0f, -15.0f, 60.0f);
	PeekRightOffset = FVector(0.0f, 15.0f, 60.0f);
	PeekRotation = 20.0f;

	IdleCameraShake = nullptr;
	WalkCameraShake = nullptr;
	RunCameraShake = nullptr;
	IdleCameraShakeClass = nullptr;
	WalkCameraShakeClass = nullptr;
	RunCameraShakeClass = nullptr;

	PlayerController = nullptr;

	MainCameraBoom = ObjectInitializer.CreateDefaultSubobject<USpringArmComponent>(this, TEXT("MainCameraBoom"));
	if (MainCameraBoom)
	{
		MainCameraBoom->SetupAttachment(GetRootComponent());
		MainCameraBoom->bUsePawnControlRotation = true;
		MainCameraBoom->bEnableCameraRotationLag = true;
		MainCameraBoom->TargetArmLength = 0.0f;
	}

	MainCamera = ObjectInitializer.CreateDefaultSubobject<UCameraComponent>(this, TEXT("MainCamera"));
	if (MainCamera)
	{
		MainCamera->SetupAttachment(MainCameraBoom);
		MainCamera->bUsePawnControlRotation = false;
	}

	PlayerSprintComponent = ObjectInitializer.CreateDefaultSubobject<UPlayerSprintComponent>(this, TEXT("PlayerSprintComponent"));
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
}

void APlayerCharacterBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	MainCameraBoom->SetRelativeTransform(CameraDefaultTransform);
}

void APlayerCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	if (PeekCurve)
	{
		float PeekCurveMinTime = 0.0f;
		float PeekCurveMaxTime = 0.0f;
		PeekCurve->GetTimeRange(PeekCurveMinTime, PeekCurveMaxTime);
		FOnTimelineFloat PeekTimelineProgress;
		PeekTimelineProgress.BindUFunction(this, FName("PeekTimelineProgress"));
		PeekTimeline.AddInterpFloat(PeekCurve, PeekTimelineProgress);

		FOnTimelineEvent PeekTimelineFinish;
		PeekTimelineFinish.BindUFunction(this, FName("PeekTimelineFinished"));
		PeekTimeline.SetTimelineFinishedFunc(PeekTimelineFinish);

		PeekTimeline.SetLooping(false);
		PeekTimeline.SetTimelineLength(PeekCurveMaxTime);
	}

	if (const UPlayerSettings* PlayerSettings = GetDefault<UPlayerSettings>())
	{
		PeekLeftOffset = PlayerSettings->PeekLeftOffset;
		PeekRightOffset = PlayerSettings->PeekRightOffset;
		PeekRotation = PlayerSettings->PeekRotation;
		MaxPeekSlowDown = PlayerSettings->MaxPeekSlowDown;
		StrafeMoveMagnitude = PlayerSettings->StrafeMoveMagnitude;
		BackMoveMagnitude = PlayerSettings->BackMoveMagnitude;
	}
	DesiredCameraLocation = CameraDefaultTransform.GetLocation();
}

void APlayerCharacterBase::PawnClientRestart()
{
	Super::PawnClientRestart();

	PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		if (auto* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->ClearAllMappings();
			if (DefaultInputContext)
				Subsystem->AddMappingContext(DefaultInputContext, 0);
		}
		StartCameraShake(IdleCameraShake, IdleCameraShakeClass);
	}
}

void APlayerCharacterBase::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	DesiredCameraLocation = CameraDefaultTransform.GetLocation() + FVector(0.0f, 0.0f, ScaledHalfHeightAdjust);
}

void APlayerCharacterBase::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	DesiredCameraLocation = CameraDefaultTransform.GetLocation() - FVector(0.0f, 0.0f, ScaledHalfHeightAdjust);
}

void APlayerCharacterBase::StartCameraShake(UCameraShakeBase*& CameraShake, const TSubclassOf<UCameraShakeBase> CameraShakeClass) const
{
	if (CameraShakeClass)
	{
		CameraShake = PlayerController->PlayerCameraManager->StartCameraShake(CameraShakeClass);
	}
}

void APlayerCharacterBase::StopCameraShake(UCameraShakeBase* CameraShake)
{
	if (CameraShake)
	{
		PlayerController->PlayerCameraManager->StopCameraShake(CameraShake);
	}
}

void APlayerCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	PeekTimeline.TickTimeline(DeltaTime);

	// Секси плавная интерполяция камеры
	DesiredCameraLocation = UKismetMathLibrary::VectorSpringInterp(DesiredCameraLocation, CameraDefaultTransform.GetLocation(),
		CameraInterpSpringState,
		240.0f, 0.8f, DeltaTime, 6.2f);
	MainCameraBoom->SetRelativeLocation(DesiredCameraLocation);
}

void APlayerCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (auto* PlayerEnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (MoveAction)
		{
			PlayerEnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Started, this,
				&APlayerCharacterBase::MoveStartActionHandler);
			PlayerEnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacterBase::MoveActionHandler);
			PlayerEnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this,
				&APlayerCharacterBase::MoveStopActionHandler);
		}
		if (LookAction)
		{
			PlayerEnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacterBase::LookActionHandler);
		}
		if (PeekAction)
		{
			PlayerEnhancedInputComponent->BindAction(PeekAction, ETriggerEvent::Started, this, &APlayerCharacterBase::PeekActionHandler);
			PlayerEnhancedInputComponent->BindAction(PeekAction, ETriggerEvent::Triggered, this, &APlayerCharacterBase::PeekActionHandler);
			PlayerEnhancedInputComponent->BindAction(PeekAction, ETriggerEvent::Completed, this, &APlayerCharacterBase::PeekStopHandler);
		}

		if (CrouchAction)
		{
			PlayerEnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this,
				&APlayerCharacterBase::CrouchActionHandler);
		}

		if (SprintAction)
		{
			PlayerEnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &APlayerCharacterBase::SprintStartHandler);
			PlayerEnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this,
				&APlayerCharacterBase::SprintStopHandler);
		}
	}
}

void APlayerCharacterBase::MoveStartActionHandler(const FInputActionValue& ActionValue)
{
	StopCameraShake(IdleCameraShake);
	if (!PlayerSprintComponent->IsSprinting())
		StartCameraShake(WalkCameraShake, WalkCameraShakeClass);
}

void APlayerCharacterBase::MoveActionHandler(const FInputActionValue& ActionValue)
{
	FVector InputAxis = ActionValue.Get<FVector>();
	if (InputAxis.X < 0)
	{
		// Движение назад
		InputAxis.X *= BackMoveMagnitude;
	}
	InputAxis.Y *= StrafeMoveMagnitude;

	if (PeekAlpha != 0.0f)
	{
		const float PeekSlowDownAmount = FMath::Abs(PeekAlpha) * MaxPeekSlowDown;
		InputAxis.X = InputAxis.X * (1.0f - PeekSlowDownAmount);
		InputAxis.Y = InputAxis.Y * (1.0f - PeekSlowDownAmount);
	}

	AddMovementInput(GetActorForwardVector(), InputAxis.X);
	AddMovementInput(GetActorRightVector(), InputAxis.Y);
}

void APlayerCharacterBase::MoveStopActionHandler(const FInputActionValue& ActionValue)
{
	StopCameraShake(WalkCameraShake);
	StopCameraShake(RunCameraShake);
	StartCameraShake(IdleCameraShake, IdleCameraShakeClass);
}

void APlayerCharacterBase::LookActionHandler(const FInputActionValue& ActionValue)
{
	const FVector InputAxis = ActionValue.Get<FVector>();
	AddControllerPitchInput(InputAxis.Y);
	AddControllerYawInput(InputAxis.X);
}

void APlayerCharacterBase::PeekActionHandler(const FInputActionValue& ActionValue)
{
	const float Magnitude = ActionValue.Get<float>();
	if (Magnitude > 0)
	{
		if (PeekState == PeekLeft)
			return;
	}
	else if (Magnitude < 0)
	{
		if (PeekState == PeekRight)
			return;
	}

	if (PeekState == Default)
	{
		PeekState = Magnitude > 0 ? PeekRight : PeekLeft;
		PeekTimeline.PlayFromStart();
	}
	else
	{
		if (PeekTimeline.IsReversing())
			PeekTimeline.Play();
	}
}

void APlayerCharacterBase::PeekStopHandler(const FInputActionValue& ActionValue)
{
	if (PeekState != Default)
		PeekTimeline.Reverse();
}

void APlayerCharacterBase::PeekTimelineProgress(float Value)
{
	if (PeekState == Default)
		return;

	PeekAlpha = Value;

	const FVector PeekLocation = PeekState == PeekRight ? PeekRightOffset : PeekLeftOffset;
	const FVector TargetLocation = PeekLocation;
	const FVector StartLocation = CameraDefaultTransform.GetLocation();
	const FVector CameraLocation = FMath::Lerp(StartLocation, TargetLocation, Value);
	MainCameraBoom->SetRelativeLocation(CameraLocation);

	if (auto* PC = Cast<APlayerController>(GetController()))
	{
		FRotator ControlRotation = PC->GetControlRotation();
		ControlRotation.Roll = Value * PeekRotation * (PeekState == PeekRight ? 1.0f : -1.0f);
		PC->SetControlRotation(ControlRotation);
	}
}

void APlayerCharacterBase::PeekTimelineFinished()
{
	if (PeekTimeline.GetPlaybackPosition() == 0.0f)
	{
		// Если таймлайн вернулся в исходную позицию сбрасываем состояние отклонения
		PeekState = Default;
		PeekAlpha = 0.0f;
	}
}

void APlayerCharacterBase::CrouchActionHandler(const FInputActionValue& ActionValue)
{
	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Crouch();
	}
}

void APlayerCharacterBase::SprintStartHandler(const FInputActionValue& ActionValue)
{
	PlayerSprintComponent->ToggleSprint(true);

	StopCameraShake(IdleCameraShake);
	StopCameraShake(WalkCameraShake);
	StartCameraShake(RunCameraShake, RunCameraShakeClass);
}

void APlayerCharacterBase::SprintStopHandler(const FInputActionValue& ActionValue)
{
	PlayerSprintComponent->ToggleSprint(false);
	StopCameraShake(RunCameraShake);
	if (GetVelocity().Length() > 10.0f)
	{
		StartCameraShake(WalkCameraShake, WalkCameraShakeClass);
	}
	else
	{
		StartCameraShake(IdleCameraShake, IdleCameraShakeClass);
	}
}
