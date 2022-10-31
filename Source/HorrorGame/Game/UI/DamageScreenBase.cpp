// It is owned by the company Dverg Verksted.

#include "DamageScreenBase.h"

void UDamageScreenBase::NativeConstruct()
{
	Super::NativeConstruct();
	bIsFocusable = false;
	if (auto* Pawn = GetOwningPlayerPawn())
	{
		Pawn->OnTakeAnyDamage.AddDynamic(this, &UDamageScreenBase::OnPlayerPawnDamageHandler);
	}
}

void UDamageScreenBase::NativeDestruct()
{
	Super::NativeDestruct();
	DamageCooldownTimer.Invalidate();
	if (auto* Pawn = GetOwningPlayerPawn())
	{
		Pawn->OnTakeAnyDamage.RemoveDynamic(this, &UDamageScreenBase::OnPlayerPawnDamageHandler);
	}
}

void UDamageScreenBase::OnPlayerPawnDamageHandler(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (bAlreadyDamaged) return;
	bAlreadyDamaged = true;
	DamageCooldownTimer.Invalidate();
	GetWorld()->GetTimerManager().SetTimer(DamageCooldownTimer, this, &UDamageScreenBase::OnDamageCooldownHandler, DamageCooldown, false, DamageCooldown);
	OnAddDamageEffect();
}

void UDamageScreenBase::OnDamageCooldownHandler()
{
	bAlreadyDamaged = false;
}
