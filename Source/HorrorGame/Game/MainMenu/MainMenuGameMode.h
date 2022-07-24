// It is owned by the company Dverg Verksted.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MainMenuCard.h"
#include "Kismet/GameplayStatics.h"
#include "MainMenuGameMode.generated.h"


UCLASS()
class HORRORGAME_API AMainMenuGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMainMenuGameMode();
	
protected:
	
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
};
