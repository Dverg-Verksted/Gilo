// It is owned by the company Dverg Verksted.

#include "Game/BotAI/BotBase.h"

#include "TimerManager.h"
#include "Game/BotAI/BotSettings.h"
#include "GameFramework/CharacterMovementComponent.h"

ABotBase::ABotBase()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
}

void ABotBase::BeginPlay()
{
	Super::BeginPlay();

	if (const UBotSettings* BotSettings = GetDefault<UBotSettings>())
	{
		CurrentWalkSpeed = GetCharacterMovement()->MaxWalkSpeed = BotSettings->WalkSpeed;
		SpeedParam = BotSettings->RunSpeedParam;
	}
	Mesh = GetMesh();
}

void ABotBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABotBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ABotBase::SetMovementSpeed()
{
	if (Time > 0)
	{
		Time -= 1;
	}
	else
	{
		Time = 7;
		GetWorldTimerManager().ClearTimer(TimerHandle);
		GetCharacterMovement()->MaxWalkSpeed = CurrentWalkSpeed;
	}
}

float ABotBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Time = 7;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &ABotBase::SetMovementSpeed, 1.0f, true);
	GetCharacterMovement()->MaxWalkSpeed = CurrentWalkSpeed / SpeedParam;
	return 0.0f;
}
