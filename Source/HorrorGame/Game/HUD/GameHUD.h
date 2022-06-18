// It is owned by the company Dverg Verksted.

#pragma once

#include "CoreMinimal.h"
#include "Game/HUD/DebugHUD.h"
#include "GameHUD.generated.h"

/**
 *
 */
UCLASS()
class HORRORGAME_API AGameHUD : public ADebugHUD
{
	GENERATED_BODY()

protected:
	// Draw the HUD.
	virtual void DrawHUD() override;
};
