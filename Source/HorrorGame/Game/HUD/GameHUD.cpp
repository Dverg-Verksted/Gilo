// It is owned by the company Dverg Verksted.

#include "Game/HUD/GameHUD.h"
#include "Game/InteractionSystem/InteractionComponent.h"
#include "Game/Player/HealthComponent.h"
#include "Game/Player/PlayerSprintComponent.h"
#include "Game/UI/GameBaseUILayer.h"
#include "Kismet/GameplayStatics.h"

AGameHUD::AGameHUD(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {}

void AGameHUD::ToggleGameMenu(bool bPauseGame)
{
	ensure(BaseUILayer);
	if (!BaseUILayer) return;

	auto* Controller = GetOwningPlayerController();
	if (!Controller) return;

	bool bMenuActive = false;
	UCommonActivatableWidget* ActiveMenuWidget;
	BaseUILayer->IsAnyMenuActivated(bMenuActive, ActiveMenuWidget);
	if (bMenuActive && ActiveMenuWidget && ActiveMenuWidget->GetClass()->IsChildOf(GameMenuClass))
	{
		ActiveMenuWidget->DeactivateWidget();
		Controller->SetInputMode(FInputModeGameOnly());
		Controller->SetShowMouseCursor(false);
		UGameplayStatics::SetGamePaused(this, false);
	}
	else
	{
		ensure(GameMenuClass);
		if (!GameMenuClass) return;

		UCommonActivatableWidget* MenuWidget = nullptr;
		BaseUILayer->PushMenu(GameMenuClass, MenuWidget);
		FInputModeUIOnly InputMode = FInputModeUIOnly();
		InputMode.SetWidgetToFocus(BaseUILayer->TakeWidget());
		Controller->SetInputMode(InputMode);
		Controller->SetShowMouseCursor(true);
		if (bPauseGame)
		{
			UGameplayStatics::SetGamePaused(this, true);
		}
	}
}

void AGameHUD::PushMenu(TSubclassOf<UCommonActivatableWidget> MenuClass, UCommonActivatableWidget*& Widget)
{
	if (!BaseUILayer) return;

	BaseUILayer->PushMenu(MenuClass, Widget);
}

void AGameHUD::PushPrompt(TSubclassOf<UCommonActivatableWidget> PromptClass, UCommonActivatableWidget*& PromptWidget)
{
	if (!BaseUILayer) return;

	BaseUILayer->PushPrompt(PromptClass, PromptWidget);
}

void AGameHUD::BeginPlay()
{
	Super::BeginPlay();

	CreateDefaultWidget();
}

void AGameHUD::CreateDefaultWidget()
{
	auto* Controller = GetOwningPlayerController();
	if (!Controller) return;

	ensure(BaseLayerClass);
	BaseUILayer = CreateWidget<UGameBaseUILayer>(Controller, BaseLayerClass, FName(TEXT("BaseUILayer")));
	ensure(BaseUILayer);
	if (!BaseUILayer) return;
	BaseUILayer->AddToViewport(0);
}

void AGameHUD::DrawHUD()
{
	Super::DrawHUD();

	if (EnableDebugHUD)
	{
		// AddText(TEXT("Test function AddText"), FText::FromString("This AddText"));
		// AddFloat(TEXT("Test function AddFloat"), 1.488f);
		// AddInt(TEXT("Test function AddInt"), 228);
		// AddBool(TEXT("Test function AddBool"), false);
		// AddBool(TEXT("Test function AddBool"), true);
		if (const auto* PlayerPawn = PlayerOwner->GetPawn())
		{
			if (const auto* SprintComponent = Cast<UPlayerSprintComponent>(PlayerPawn->GetComponentByClass(UPlayerSprintComponent::StaticClass())))
			{
				AddFloat(TEXT("Выносливость:"), SprintComponent->CurrentStamina);
				AddFloat(TEXT("Скорость:"), PlayerPawn->GetVelocity().Length());
			}

			if (const auto* Interaction = Cast<UInteractionComponent>(PlayerPawn->GetComponentByClass(UInteractionComponent::StaticClass())))
			{
				FString Msg = TEXT("Нет");
				if (const auto* CurrentActor = Interaction->GetCurrentInteractiveObject())
				{
					Msg = CurrentActor->GetName();
				}
				AddText(TEXT("Интерактивный объект"), FText::FromString(Msg));
			}

			if (const auto* HealthComp = Cast<UHealthComponent>(PlayerPawn->GetComponentByClass(UHealthComponent::StaticClass())))
			{
				AddFloat(TEXT("Здоровье:"), HealthComp->GetHealth());
			}
		}
	}
}
