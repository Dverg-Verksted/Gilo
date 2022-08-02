// It is owned by the company Dverg Verksted.

#include "Game/MainMenu/MainMenuGameMode.h"
#include "Game/MainMenu/MainMenuPlayerController.h"

AMainMenuGameMode::AMainMenuGameMode()
{
	PlayerControllerClass = AMainMenuPlayerController::StaticClass();
}

void AMainMenuGameMode::BeginPlay() {}

void AMainMenuGameMode::Tick(float DeltaTime) {}
