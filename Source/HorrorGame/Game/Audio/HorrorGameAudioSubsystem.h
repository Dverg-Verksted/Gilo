// It is owned by the company Dverg Verksted.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "HorrorGameAudioSubsystem.generated.h"

class USoundControlBusMix;
class USoundControlBus;
class USoundSubmix;
class USoundEffectSubmixPreset;

/**
 * Аудио подсистема игры
 */
UCLASS()
class HORRORGAME_API UHorrorGameAudioSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	void ApplyVolumeSettings(UObject* WorldContextObject);
	// USubsystem implementation Begin
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	// USubsystem implementation End
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	/** Called once all UWorldSubsystems have been initialized */
	virtual void PostInitialize() override;
	void UpdateUserMix(const UWorld* World);
	/** Called when world is ready to start gameplay before the game mode transitions to the correct state and call BeginPlay on all actors */
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

private:
	// Called when determining whether to create this Subsystem
	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override;
	// Default Sound Control Bus Mix retrieved from the Lyra Audio Settings
	UPROPERTY(Transient)
	USoundControlBusMix* DefaultBaseMix = nullptr;
	// User Sound Control Bus Mix retrieved from the Lyra Audio Settings
	UPROPERTY(Transient)
	USoundControlBusMix* UserMix = nullptr;
	// Overall Sound Control Bus retrieved from the Lyra Audio Settings and linked to the UI and game settings in LyraSettingsLocal
	UPROPERTY(Transient)
	USoundControlBus* OverallControlBus = nullptr;

	// Music Sound Control Bus retrieved from the Lyra Audio Settings and linked to the UI and game settings in LyraSettingsLocal
	UPROPERTY(Transient)
	USoundControlBus* MusicControlBus = nullptr;

	// SoundFX Sound Control Bus retrieved from the Lyra Audio Settings and linked to the UI and game settings in LyraSettingsLocal
	UPROPERTY(Transient)
	USoundControlBus* SoundFXControlBus = nullptr;
};
