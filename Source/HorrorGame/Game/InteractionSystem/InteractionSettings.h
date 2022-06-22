// It is owned by the company Dverg Verksted.

#pragma once

#include "CoreMinimal.h"
#include "DataRegistryId.h"
#include "Engine/DeveloperSettings.h"
#include "InteractionSettings.generated.h"

/**
 * Настройки интерактивной системы
 */
UCLASS(Config=Game, defaultconfig, meta = (DisplayName="Настройки взаимодействия"))
class HORRORGAME_API UInteractionSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UInteractionSettings();

	/* Типы объектов для трассировки */ 
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Трассировка", DisplayName="Типы объектов")
	TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjectTypes;

	/* Профиль коллизий интерактивного объекта */ 
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Трассировка", DisplayName="Профиль коллизий")
	FName CollisionProfile;
	
	static FORCEINLINE UInteractionSettings* Get()
	{
		UInteractionSettings* Settings = GetMutableDefault<UInteractionSettings>();
		check(Settings);
		return Settings;
	}
};
