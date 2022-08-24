// It is owned by the company Dverg Verksted.

#include "GenericPromptBase.h"

UGenericPromptBase::UGenericPromptBase() {}

void UGenericPromptBase::InitPrompt_Implementation(const FPromptSetup& Setup)
{
	if (Setup.Timeout > 0.0f)
	{
		GetWorld()->GetTimerManager().SetTimer(TimeoutTimerHandle, this, &UGenericPromptBase::OnTimeoutHandler, Setup.Timeout, false);
	}
}

void UGenericPromptBase::ClearTimer()
{
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}

void UGenericPromptBase::ConfirmPrompt()
{
	ClearTimer();
	OnPromptConfirm.Broadcast();
	DeactivateWidget();
}

void UGenericPromptBase::CancelPrompt()
{
	ClearTimer();
	OnPromptCancel.Broadcast();
	DeactivateWidget();
}

void UGenericPromptBase::OnTimeoutHandler()
{
	ClearTimer();
	OnPromptTimeout.Broadcast();
	DeactivateWidget();
}
