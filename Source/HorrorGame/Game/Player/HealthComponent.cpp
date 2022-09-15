// It is owned by the company Dverg Verksted.

#include "HealthComponent.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UHealthComponent::SetHealth(float InHealth)
{
	ensure(MaxHealth > 0);
	if (MaxHealth <= 0) return;

	Health = InHealth;
	if (Health <= 0.0f)
	{
		OnDeath.Broadcast();
	}
	else
	{
		OnHealthChanged.Broadcast(Health / MaxHealth);
	}
}

void UHealthComponent::OnOwnerTakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	UpdateHealth(-FMath::Abs(Damage));
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	if (auto* OwnerActor = GetOwner())
	{
		OwnerActor->OnTakeAnyDamage.AddDynamic(this, &ThisClass::OnOwnerTakeDamage);
	}
}

void UHealthComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (auto* OwnerActor = GetOwner())
	{
		OwnerActor->OnTakeAnyDamage.RemoveDynamic(this, &ThisClass::OnOwnerTakeDamage);
	}
}

void UHealthComponent::UpdateHealth(float InAmount)
{
	const float NewHealth = FMath::Clamp(Health + InAmount, 0.0f, MaxHealth);
	SetHealth(NewHealth);
}
