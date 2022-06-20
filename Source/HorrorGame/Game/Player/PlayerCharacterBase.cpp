// It is owned by the company Dverg Verksted.


#include "Game/Player/PlayerCharacterBase.h"

#include "EnhancedInputSubsystems.h"
#include "PlayerSettings.h"
#include "GameFramework/CharacterMovementComponent.h"

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
}

void APlayerCharacterBase::PawnClientRestart()
{
	Super::PawnClientRestart();

	if (const auto* PC = Cast<APlayerController>(GetController()))
	{
		if (auto* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->ClearAllMappings();
			if (DefaultInputContext)
				Subsystem->AddMappingContext(DefaultInputContext, 0);
		}
	}
}

void APlayerCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	PeekTimeline.TickTimeline(DeltaTime);
}

void APlayerCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (auto* PlayerEnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (MoveAction)
			PlayerEnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacterBase::MoveActionHandler);

		if (LookAction)
			PlayerEnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacterBase::LookActionHandler);

		if (PeekAction)
		{
			PlayerEnhancedInputComponent->BindAction(PeekAction, ETriggerEvent::Started, this, &APlayerCharacterBase::PeekActionHandler);
			PlayerEnhancedInputComponent->BindAction(PeekAction, ETriggerEvent::Triggered, this, &APlayerCharacterBase::PeekActionHandler);
			PlayerEnhancedInputComponent->BindAction(PeekAction, ETriggerEvent::Completed, this, &APlayerCharacterBase::PeekStopHandler);
		}

		if (CrouchAction)
			PlayerEnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this,
				&APlayerCharacterBase::CrouchActionHandler);

		if (SprintAction)
		{
			PlayerEnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &APlayerCharacterBase::SprintStartHandler);
			PlayerEnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this,
				&APlayerCharacterBase::SprintStopHandler);
		}
	}
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
	if (auto* CharMovement = GetCharacterMovement())
	{
		if (CharMovement->IsCrouching())
		{
			CharMovement->UnCrouch();
		}
		else
		{
			CharMovement->Crouch();
		}
	}
}

void APlayerCharacterBase::SprintStartHandler(const FInputActionValue& ActionValue)
{
	PlayerSprintComponent->ToggleSprint(true);
}

void APlayerCharacterBase::SprintStopHandler(const FInputActionValue& ActionValue)
{
	PlayerSprintComponent->ToggleSprint(false);
}
