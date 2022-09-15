// Copyright Epic Games, Inc. All Rights Reserved.

#include "HorrorGameGameModeBase.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/Character.h"

void AHorrorGameGameModeBase::KillPlayer(APlayerController* PlayerController)
{
	if (!PlayerController) return;
	if (auto* PlayerChar = PlayerController->GetCharacter())
	{
		// Убиваем персонаж игрока
		PlayerChar->Destroy();
	}
	ensure(DeathScreenClass);
	if (!DeathScreenClass) return;
	DestroyDeathScreenWidget();
	DeathScreenWidget = CreateWidget<UUserWidget>(PlayerController, DeathScreenClass, FName(TEXT("DeathScreenWidget")));
	ensure(DeathScreenWidget);
	if (!DeathScreenWidget) return;

	DeathScreenWidget->AddToViewport(DeathScreenZOrder);
	FInputModeUIOnly InputMode;
	InputMode.SetWidgetToFocus(DeathScreenWidget->TakeWidget());
	PlayerController->SetInputMode(InputMode);
	PlayerController->SetShowMouseCursor(true);
}

void AHorrorGameGameModeBase::DestroyDeathScreenWidget()
{
	if (DeathScreenWidget)
	{
		DeathScreenWidget->RemoveFromParent();
		DeathScreenWidget = nullptr;
	}
}

void AHorrorGameGameModeBase::FinishRestartPlayer(AController* NewPlayer, const FRotator& StartRotation)
{
	Super::FinishRestartPlayer(NewPlayer, StartRotation);
	DestroyDeathScreenWidget();
	if (auto* PlayerController = Cast<APlayerController>(NewPlayer))
	{
		PlayerController->SetInputMode(FInputModeGameOnly());
		PlayerController->SetShowMouseCursor(false);
	}
}
