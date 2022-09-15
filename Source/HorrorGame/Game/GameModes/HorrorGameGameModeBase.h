// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "HorrorGameGameModeBase.generated.h"

/**
 *
 */
UCLASS()
class HORRORGAME_API AHorrorGameGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	/* Убивает текущего игрока и показывает экран смерти */
	UFUNCTION(BlueprintCallable)
	void KillPlayer(APlayerController* PlayerController);
	void DestroyDeathScreenWidget();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "DeathScreen")
	TSubclassOf<UUserWidget> DeathScreenClass;
	UPROPERTY(EditDefaultsOnly, Category = "DeathScreen")
	int32 DeathScreenZOrder = 1500;
	UPROPERTY()
	UUserWidget* DeathScreenWidget = nullptr;
	virtual void FinishRestartPlayer(AController* NewPlayer, const FRotator& StartRotation) override;
};
