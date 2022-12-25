// It is owned by the company Dverg Verksted.

#include "HealthComponent.h"

#include "PlayerSettings.h"

#pragma optimize("", off)

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UHealthComponent::SetHealth(float InHealth)
{
	ensure(MaxHealth > 0);
	if (MaxHealth <= 0 || !bAlive) return;

	Health = FMath::Clamp(InHealth, 0.0f, MaxHealth);
	if (Health <= 0.0f)
	{
		bAlive = false;
		OnDeath.Broadcast();
		ClearEvents();
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
	ClearHealthRegenTimer();
	if (!bAlive) return;
	UpdateHealth(-FMath::Abs(Damage));
	if (HealthRegenDelay > 0.0f)
	{
		FTimerManager& TimerManager = GetWorld()->GetTimerManager();
		TimerManager.SetTimer(RegenTimerHandle, FTimerDelegate::CreateUObject(this, &UHealthComponent::SetHealth, MaxHealth), HealthRegenDelay, false, HealthRegenDelay);
	}
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	if (auto* OwnerActor = GetOwner())
	{
		OwnerActor->OnTakeAnyDamage.AddDynamic(this, &ThisClass::OnOwnerTakeDamage);
	}
}

void UHealthComponent::Initialize(const float& InMaxHealth, const float& InHealthRegenDelay)
{
	MaxHealth = InMaxHealth;
	HealthRegenDelay = InHealthRegenDelay;
	bAlive = MaxHealth > 0.0f;
	SetHealth(MaxHealth);
}

void UHealthComponent::ClearEvents()
{
	OnDeath.Clear();
	OnHealthChanged.Clear();
	ClearHealthRegenTimer();
	if (auto* OwnerActor = GetOwner())
	{
		OwnerActor->OnTakeAnyDamage.RemoveDynamic(this, &ThisClass::OnOwnerTakeDamage);
	}
}

void UHealthComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	ClearEvents();
}

void UHealthComponent::UpdateHealth(float InAmount)
{
	SetHealth(Health + InAmount);
}

#pragma optimize("", on)
