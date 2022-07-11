// It is owned by the company Dverg Verksted.

#include "Game/Player/PlayerCharacterBase.h"
#include "EnhancedInputSubsystems.h"
#include "PlayerSettings.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Game/Settings/HorrorSettingsLocal.h"
#include "Game/System/HorrorAssetManager.h"

APlayerCharacterBase::APlayerCharacterBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	StrafeMoveMagnitude = 0.75f;
	BackMoveMagnitude = 0.6f;
	MaxPeekSlowDown = 0.3f;
	PeekAlpha = 0.0f;
	PeekState = Default;

	PeekLeftOffset = FVector(0.0f, -15.0f, -12.0f);
	PeekRightOffset = FVector(0.0f, 15.0f, -12.0f);
	PeekRotation = 20.0f;

	CameraCrouchOffset = FVector::ZeroVector;
	CameraPeekOffset = FVector::ZeroVector;

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

	WalkCameraShakeComponent = ObjectInitializer.CreateDefaultSubobject<UWalkCameraShakeComponent>(this, TEXT("WalkCameraShakeComponent"));
	InteractionComponent = ObjectInitializer.CreateDefaultSubobject<UInteractionComponent>(this, TEXT("InteractionComponent"));
	PhysicsHandleComponent = ObjectInitializer.CreateDefaultSubobject<UPhysicsHandleComponent>(this, TEXT("PhysicsHandleComponent"));
}

void APlayerCharacterBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
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
	CameraDefaultTransform = MainCameraBoom->GetRelativeTransform();
}

bool APlayerCharacterBase::InitInputMappings() const
{
	if (!PlayerController) return false;

	const auto* GameSettings = UHorrorSettingsLocal::Get();
	ensure(GameSettings);
	if (!GameSettings) return false;

	auto* PlayerSettings = UPlayerSettings::Get();
	ensure(PlayerSettings);
	if (!PlayerSettings) return false;

	const auto* AssetManager = UHorrorAssetManager::Get();
	ensure(AssetManager);
	if (!AssetManager) return false;

	const auto* MappingConfig = Cast<UPlayerMappableInputConfig>(AssetManager->LoadAssetSync(PlayerSettings->DefaultKeyboardMapping));
	ensure(MappingConfig);
	if (!MappingConfig) return false;

	if (auto* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
	{
		Subsystem->ClearAllMappings();
		GameSettings->AddDefaultMappings(MappingConfig, Subsystem);
		return true;
	}
	return false;
}

void APlayerCharacterBase::PawnClientRestart()
{
	Super::PawnClientRestart();
	InteractionComponent->StopTrace();
	PlayerController = Cast<APlayerController>(GetController());
	if (InitInputMappings())
	{
		InteractionComponent->OnPlayerReady();
		InteractionComponent->StartTrace();
	}
}

void APlayerCharacterBase::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	CameraCrouchOffset = FVector(0.0f, 0.0f, -ScaledHalfHeightAdjust);
}

void APlayerCharacterBase::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	CameraCrouchOffset = FVector::ZeroVector;
}

void APlayerCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	PeekTimeline.TickTimeline(DeltaTime);

	FVector DesiredCameraLocation = CameraDefaultTransform.GetLocation();
	DesiredCameraLocation += CameraCrouchOffset;
	DesiredCameraLocation += CameraPeekOffset;

	// Секси плавная интерполяция камеры
	const FVector ResultCameraLocation = UKismetMathLibrary::VectorSpringInterp(MainCameraBoom->GetRelativeLocation(), DesiredCameraLocation, CameraInterpSpringState, 800.0f, 0.85f, DeltaTime, 6.2f);
	MainCameraBoom->SetRelativeLocation(ResultCameraLocation);
}

void APlayerCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	auto* PlayerEnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	ensure(PlayerEnhancedInputComponent);

	if (!PlayerEnhancedInputComponent) return;

	if (MoveAction)
	{
		PlayerEnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Started, this, &APlayerCharacterBase::MoveStartActionHandler);
		PlayerEnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacterBase::MoveActionHandler);
		PlayerEnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &APlayerCharacterBase::MoveStopActionHandler);
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
		PlayerEnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &APlayerCharacterBase::CrouchActionHandler);
	}

	if (SprintAction)
	{
		PlayerEnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &APlayerCharacterBase::SprintStartHandler);
		PlayerEnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &APlayerCharacterBase::SprintStopHandler);
	}
}

void APlayerCharacterBase::MoveStartActionHandler(const FInputActionValue& ActionValue)
{
	PlayerSprintComponent->ToggleMoveActionInput(true);
	WalkCameraShakeComponent->ToggleMoveAction(true);
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

	if (InputAxis.X != 0.0f)
	{
		AddMovementInput(GetActorForwardVector(), InputAxis.X);
		bMoveInputActive = true;
	}
	if (InputAxis.Y != 0.0f)
	{
		AddMovementInput(GetActorRightVector(), InputAxis.Y);
		bMoveInputActive = true;
	}
}

void APlayerCharacterBase::MoveStopActionHandler(const FInputActionValue& ActionValue)
{
	bMoveInputActive = false;
	PlayerSprintComponent->ToggleMoveActionInput(false);
	WalkCameraShakeComponent->ToggleMoveAction(false);
}

void APlayerCharacterBase::LookActionHandler(const FInputActionValue& ActionValue)
{
	const FVector InputAxis = ActionValue.Get<FVector>();
	if (InputAxis.Y != 0.0f) AddControllerPitchInput(InputAxis.Y);
	if (InputAxis.X != 0.0f) AddControllerYawInput(InputAxis.X);
}

void APlayerCharacterBase::PeekActionHandler(const FInputActionValue& ActionValue)
{
	const float Magnitude = ActionValue.Get<float>();
	if (Magnitude > 0)
	{
		if (PeekState == PeekLeft) return;
	}
	else if (Magnitude < 0)
	{
		if (PeekState == PeekRight) return;
	}

	if (PeekState == Default)
	{
		PeekState = Magnitude > 0 ? PeekRight : PeekLeft;
		PeekTimeline.PlayFromStart();
	}
	else
	{
		if (PeekTimeline.IsReversing()) PeekTimeline.Play();
	}
}

void APlayerCharacterBase::PeekStopHandler(const FInputActionValue& ActionValue)
{
	if (PeekState != Default) PeekTimeline.Reverse();
}

void APlayerCharacterBase::PeekTimelineProgress(float Value)
{
	if (PeekState == Default) return;

	PeekAlpha = Value;

	const FVector PeekOffset = PeekState == PeekRight ? PeekRightOffset : PeekLeftOffset;
	const FVector CameraLocation = FMath::Lerp(FVector::ZeroVector, PeekOffset, Value);

	CameraPeekOffset = CameraLocation;

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
	WalkCameraShakeComponent->ToggleSprinting(true);
}

void APlayerCharacterBase::SprintStopHandler(const FInputActionValue& ActionValue)
{
	PlayerSprintComponent->ToggleSprint(false);
	WalkCameraShakeComponent->ToggleSprinting(false);
}
