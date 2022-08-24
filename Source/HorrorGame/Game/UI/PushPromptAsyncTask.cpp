// It is owned by the company Dverg Verksted.

#include "PushPromptAsyncTask.h"
#include "Game/HUD/GameHUD.h"

UPushPromptAsyncTask::UPushPromptAsyncTask(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	WorldContextObject = nullptr;
}

UPushPromptAsyncTask* UPushPromptAsyncTask::ShowPrompt(UObject* WorldContext, APlayerController* PlayerController, TSubclassOf<UGenericPromptBase> Class, const FPromptSetup& Setup)
{
	if (auto* NewTask = NewObject<UPushPromptAsyncTask>(WorldContext))
	{
		NewTask->WorldContextObject = WorldContext;
		NewTask->PlayerControllerWeakPtr = PlayerController;
		NewTask->PromptClass = Class;
		NewTask->PromptSetup = Setup;
		return NewTask;
	}
	return nullptr;
}

void UPushPromptAsyncTask::Activate()
{
	Super::Activate();

	const APlayerController* Controller = PlayerControllerWeakPtr.Get();
	if (!Controller)
	{
		FFrame::KismetExecutionMessage(TEXT("Не задан PlayerController для диалога!!!"), ELogVerbosity::Warning);
		return;
	}

	if (auto* HUD = Cast<AGameHUD>(Controller->GetHUD()))
	{
		UCommonActivatableWidget* Widget = nullptr;
		HUD->PushPrompt(PromptClass, Widget);
		PromptWidget = Cast<UGenericPromptBase>(Widget);
		if (PromptWidget)
		{
			PromptWidget->InitPrompt(PromptSetup);
			BindToPrompt();
		}
	}
}

void UPushPromptAsyncTask::OnConfirmHandler()
{
	Confirm.Broadcast();
	UnbindFromPrompt();
}

void UPushPromptAsyncTask::OnCancelHandler()
{
	Cancel.Broadcast();
	UnbindFromPrompt();
}

void UPushPromptAsyncTask::OnTimeoutHandler()
{
	Timeout.Broadcast();
	UnbindFromPrompt();
}

void UPushPromptAsyncTask::BindToPrompt()
{
	if (PromptWidget)
	{
		PromptWidget->OnPromptConfirm.AddDynamic(this, &UPushPromptAsyncTask::OnConfirmHandler);
		PromptWidget->OnPromptCancel.AddDynamic(this, &UPushPromptAsyncTask::OnCancelHandler);
		PromptWidget->OnPromptTimeout.AddDynamic(this, &UPushPromptAsyncTask::OnTimeoutHandler);
	}
}

void UPushPromptAsyncTask::UnbindFromPrompt()
{
	if (PromptWidget)
	{
		PromptWidget->OnPromptConfirm.RemoveDynamic(this, &UPushPromptAsyncTask::OnConfirmHandler);
		PromptWidget->OnPromptCancel.RemoveDynamic(this, &UPushPromptAsyncTask::OnCancelHandler);
		PromptWidget->OnPromptTimeout.RemoveDynamic(this, &UPushPromptAsyncTask::OnTimeoutHandler);
	}
}
