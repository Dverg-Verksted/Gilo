// It is owned by the company Dverg Verksted.

#pragma once

#include "CoreMinimal.h"
#include "DataRegistryId.h"
#include "Engine/DeveloperSettings.h"
#include "InteractionSettings.generated.h"

/**
 * Настройки интерактивной системы
 */
UCLASS(Config = Game, defaultconfig, meta = (DisplayName = "Настройки взаимодействия"))
class HORRORGAME_API UInteractionSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UInteractionSettings();

	/* Дистанция трассировки */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Трассировка", DisplayName = "Дистанция", meta = (ToolTip = "Дистанция трассировки", ClampMin = 10.0f, UIMin = 10.0f))
	float TraceDistance;

	/* Радиус трассировки */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Трассировка", DisplayName = "Радиус", meta = (ToolTip = "Радиус сферы трассировки", ClampMin = 0.5f, UIMin = 0.5f))
	float TraceRadius;

	/* Типы объектов для трассировки */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Трассировка", DisplayName = "Типы объектов")
	TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjectTypes;

	/* Профиль коллизий интерактивного объекта */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Трассировка", DisplayName = "Профиль коллизий")
	FName CollisionProfile;

	static FORCEINLINE const UInteractionSettings* Get()
	{
		const UInteractionSettings* Settings = GetDefault<UInteractionSettings>();
		ensure(Settings);
		return Settings;
	}
};
