// It is owned by the company Dverg Verksted.

#include "Game/HUD/GameHUD.h"

#include "Game/InteractionSystem/InteractionComponent.h"
#include "Game/Player/PlayerSprintComponent.h"

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
			if (const auto* SprintComponent = Cast<UPlayerSprintComponent>(
				PlayerPawn->GetComponentByClass(UPlayerSprintComponent::StaticClass())))
			{
				AddFloat(TEXT("Выносливость:"), SprintComponent->CurrentStamina);
				AddFloat(TEXT("Скорость:"), PlayerPawn->GetVelocity().Length());
			}

			if (const auto* Interaction = Cast<
				UInteractionComponent>(PlayerPawn->GetComponentByClass(UInteractionComponent::StaticClass())))
			{
				FString Msg = TEXT("Нет");
				if (const auto* CurrentActor = Interaction->GetCurrentInteractiveObject())
				{
					Msg = CurrentActor->GetName();
				}
				AddText(TEXT("Интерактивный объект"), FText::FromString(Msg));
			}
		}
	}
}
