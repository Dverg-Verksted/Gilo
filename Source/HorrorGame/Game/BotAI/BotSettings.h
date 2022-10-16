// It is owned by the company Dverg Verksted.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "BotSettings.generated.h"

/**
 * Настройки бота
 */
UCLASS(Config = Game, defaultconfig, meta = (DisplayName = "Настройки бота"))
class HORRORGAME_API UBotSettings : public UDeveloperSettings
{
	GENERATED_BODY()
public:
	UBotSettings();
	/* Скорость ходьбы */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Перемещение", DisplayName = "Скорость ходьбы")
	float WalkSpeed;

	/* Модификатор скорость бега */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Перемещение", DisplayName = "Коэф. замедления бега", meta = (ToolTip = "Коэффициент замедления при беге", ClampMin = 1.0f, UIMin = 1.0f))
	float RunSpeedParam;

	UPROPERTY(Config, EditAnywhere,BlueprintReadWrite,Category = "AI")
	float ISightRadius;

	UPROPERTY(Config, EditAnywhere,BlueprintReadWrite,Category = "AI")
	float ISightAge;
	
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "AI", meta = (ToolTip = "Радиус начала потери видимости", ClampMax = 4000.0f, ClampMin = 1500.0f, UIMax = 4000.0f, UIMin = 1500.0f))
	float ILoseSightRadius;

	UPROPERTY(Config, EditAnywhere,BlueprintReadWrite,Category = "AI")
	float IFieldOfView;

	UPROPERTY(Config, EditAnywhere,BlueprintReadWrite,Category = "AI")
	bool bIsPlayerDetected;

	UPROPERTY(Config, EditAnywhere,BlueprintReadWrite,Category = "AI")
	float DistanceToPlayer;

	
	static FORCEINLINE const UBotSettings* Get()
	{
		const UBotSettings* Settings = GetDefault<UBotSettings>();
		ensure(Settings);
		return Settings;
	}
};
