// It is owned by the company Dverg Verksted.


#include "Game/Player/PlayerSprintComponent.h"

#include "PlayerSettings.h"
#include "GameFramework/Character.h"

UPlayerSprintComponent::UPlayerSprintComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	OwnerCharacter = nullptr;
	CharacterMovement = nullptr;

	WalkSpeed = 400.0f;
	RunSpeed = 800.0f;

	bSprinting = false;
	bExhausted = false;

	MaxStaminaAmount = 15.0f;
	MinStaminaAmount = 3.75f;
	StaminaDrainRate = 1.0f;
	StaminaRestoreRate = 0.3f;
	CurrentStamina = MaxStaminaAmount;
}


void UPlayerSprintComponent::BeginPlay()
{
	Super::BeginPlay();

	if (auto* Character = Cast<ACharacter>(GetOwner()))
	{
		OwnerCharacter = Character;
		CharacterMovement = Character->GetCharacterMovement();
	}

	if (const UPlayerSettings* PlayerSettings = GetDefault<UPlayerSettings>())
	{
		WalkSpeed = PlayerSettings->WalkSpeed;
		RunSpeed = PlayerSettings->RunSpeed;
		MaxStaminaAmount = PlayerSettings->MaxStaminaAmount;
		MinStaminaAmount = PlayerSettings->MinStaminaAmount;
		StaminaDrainRate = PlayerSettings->StaminaDrainRate;
		StaminaRestoreRate = PlayerSettings->StaminaRestoreRate;
		CurrentStamina = MaxStaminaAmount;
	}
}

void UPlayerSprintComponent::ToggleSprint(bool bSprintEnabled)
{
	bSprinting = bSprintEnabled;
}


void UPlayerSprintComponent::UpdateStamina(float DeltaTime)
{
	bool bNewExhausted = bExhausted;
	float NewStaminaValue = CurrentStamina;
	if (ensure(CharacterMovement))
	{
		if (bSprinting && !bExhausted && CurrentStamina > 0.0f)
		{
			NewStaminaValue = FMath::FInterpConstantTo(CurrentStamina, 0.0f, DeltaTime, StaminaDrainRate);
			if (NewStaminaValue == 0.0f)
			{
				// Персонаж вымотался
				bNewExhausted = true;
			}
			CharacterMovement->MaxWalkSpeed = NewStaminaValue > 0.0f ? RunSpeed : WalkSpeed;
		}
		else
		{
			NewStaminaValue = FMath::FInterpConstantTo(CurrentStamina, MaxStaminaAmount, DeltaTime, StaminaRestoreRate);
			if (bExhausted)
			{
				if (NewStaminaValue >= MinStaminaAmount)
				{
					// Персонаж восстановил достаточно сил для возобновления бега
					bNewExhausted = false;
				}
			}
			CharacterMovement->MaxWalkSpeed = WalkSpeed;
		}
	}
	if (NewStaminaValue != CurrentStamina)
	{
		CurrentStamina = NewStaminaValue;
		OnStaminaChanged.Broadcast(CurrentStamina);
	}
	if (bNewExhausted != bExhausted)
	{
		bExhausted = bNewExhausted;
		OnExhaustedChanged.Broadcast(bExhausted, CurrentStamina);
	}
}

void UPlayerSprintComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	UpdateStamina(DeltaTime);
}
