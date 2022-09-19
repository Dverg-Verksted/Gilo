// It is owned by the company Dverg Verksted.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "HorrorGameAudioSettings.generated.h"

/**
 * Настройки звуковой подсистемы игры
 */
UCLASS(config = Game, defaultconfig, meta = (DisplayName = "HorrorGameAudioSettings"))
class HORRORGAME_API UHorrorGameAudioSettings : public UDeveloperSettings
{
	GENERATED_BODY()
public:
	/** The Default Base Control Bus Mix */
	UPROPERTY(config, EditAnywhere, Category = MixSettings, meta = (AllowedClasses = "SoundControlBusMix"))
	FSoftObjectPath DefaultControlBusMix;

	/** The Default Base Control Bus Mix */
	UPROPERTY(config, EditAnywhere, Category = UserMixSettings, meta = (AllowedClasses = "SoundControlBusMix"))
	FSoftObjectPath UserSettingsControlBusMix;

	/** Общая шина */
	UPROPERTY(config, EditAnywhere, Category = UserMixSettings, meta = (AllowedClasses = "SoundControlBus"))
	FSoftObjectPath OverallVolumeControlBus;

	/** Звуковая шина для музыки */
	UPROPERTY(config, EditAnywhere, Category = UserMixSettings, meta = (AllowedClasses = "SoundControlBus"))
	FSoftObjectPath MusicVolumeControlBus;

	/** Звуковая шина эффектов */
	UPROPERTY(config, EditAnywhere, Category = UserMixSettings, meta = (AllowedClasses = "SoundControlBus"))
	FSoftObjectPath FXVolumeControlBus;
};
