// It is owned by the company Dverg Verksted.

#include "HealthComponent.h"

#include "PlayerSettings.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UHealthComponent::SetHealth(float InHealth)
{
	ensure(MaxHealth > 0);
	if (MaxHealth <= 0) return;

	Health = FMath::Clamp(InHealth, 0.0f, MaxHealth);
	if (Health <= 0.0f)
	{
		OnDeath.Broadcast();
	}
	else
	{
		OnHealthChanged.Broadcast(Health / MaxHealth);
	}
}

void UHealthComponent::ClearHealthRegenTimer()
{
	if (RegenTimerHandle.IsValid())
	{
		RegenTimerHandle.Invalidate();
	}
}

void UHealthComponent::OnOwnerTakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	UpdateHealth(-FMath::Abs(Damage));
	ClearHealthRegenTimer();
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	TimerManager.SetTimer(RegenTimerHandle, FTimerDelegate::CreateLambda([&]() { SetHealth(MaxHealth); }), HealthRegenDelay, false, HealthRegenDelay);
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	if (auto* OwnerActor = GetOwner())
	{
		OwnerActor->OnTakeAnyDamage.AddDynamic(this, &ThisClass::OnOwnerTakeDamage);
	}
	if (const auto* PlayerSettings = GetDefault<UPlayerSettings>())
	{
		MaxHealth = PlayerSettings->MaxHealth;
		HealthRegenDelay = PlayerSettings->HealthRegenDelay;
		SetHealth(MaxHealth);
	}
}

void UHealthComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	ClearHealthRegenTimer();
	if (auto* OwnerActor = GetOwner())
	{
		OwnerActor->OnTakeAnyDamage.RemoveDynamic(this, &ThisClass::OnOwnerTakeDamage);
	}
}

void UHealthComponent::UpdateHealth(float InAmount)
{
	SetHealth(Health + InAmount);
}
