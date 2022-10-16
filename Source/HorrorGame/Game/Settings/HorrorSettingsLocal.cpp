// It is owned by the company Dverg Verksted.

#include "Game/Settings/HorrorSettingsLocal.h"

#include "AudioModulationStatics.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "SoundControlBus.h"
#include "SoundControlBusMix.h"
#include "Game/Audio/HorrorGameAudioSettings.h"
#include "Game/Player/PlayerSettings.h"
#include "Game/System/HorrorAssetManager.h"

UHorrorSettingsLocal::UHorrorSettingsLocal() {}

UHorrorSettingsLocal* UHorrorSettingsLocal::Get()
{
	return GEngine ? CastChecked<UHorrorSettingsLocal>(GEngine->GetGameUserSettings()) : nullptr;
}

void UHorrorSettingsLocal::SetOverallVolume(float InVolume)
{
	AudioVolume_Overall = InVolume;
	// Check to see if references to the control buses and control bus mixes have been loaded yet
	// Will likely need to be loaded if this function is the first time a setter has been called from the UI
	if (!bSoundControlBusMixLoaded)
	{
		LoadAudioControlBusses();
	}

	ensureMsgf(bSoundControlBusMixLoaded, TEXT("UserControlBusMix Settings Failed to Load."));

	if (OverallControlBus)
	{
		SetVolumeForControlBus(OverallControlBus, AudioVolume_Overall);
	}
}

float UHorrorSettingsLocal::GetMusicVolume() const
{
	return AudioVolume_Music;
}

void UHorrorSettingsLocal::SetMusicVolume(float InVolume)
{
	AudioVolume_Music = InVolume;
	// Check to see if references to the control buses and control bus mixes have been loaded yet
	// Will likely need to be loaded if this function is the first time a setter has been called from the UI
	if (!bSoundControlBusMixLoaded)
	{
		LoadAudioControlBusses();
	}

	ensureMsgf(bSoundControlBusMixLoaded, TEXT("UserControlBusMix Settings Failed to Load."));

	if (MusicControlBus)
	{
		SetVolumeForControlBus(MusicControlBus, AudioVolume_Music);
	}
}

void UHorrorSettingsLocal::SetFXVolume(float InVolume)
{
	AudioVolume_FX = InVolume;
	// Check to see if references to the control buses and control bus mixes have been loaded yet
	// Will likely need to be loaded if this function is the first time a setter has been called from the UI
	if (!bSoundControlBusMixLoaded)
	{
		LoadAudioControlBusses();
	}

	ensureMsgf(bSoundControlBusMixLoaded, TEXT("UserControlBusMix Settings Failed to Load."));

	if (SoundFXControlBus)
	{
		SetVolumeForControlBus(SoundFXControlBus, AudioVolume_FX);
	}
}

void UHorrorSettingsLocal::UpdateKeyboardBinding(APlayerController* PlayerController, const FName MappingName, const FKey NewKey, bool bSaveSettings)
{
	ensure(PlayerController);
	if (!PlayerController) return;

	auto* GameSettings = Get();
	ensure(GameSettings);
	if (!GameSettings) return;

	if (const auto* LocalPlayer = PlayerController->GetLocalPlayer())
	{
		GameSettings->AddOrUpdateCustomKeyboardBindings(MappingName, NewKey, LocalPlayer);
		if (bSaveSettings) GameSettings->ApplySettings(false);
	}
}

void UHorrorSettingsLocal::SaveUserSettings()
{
	auto* GameSettings = Get();
	ensure(GameSettings);
	if (!GameSettings) return;
	GameSettings->ApplySettings(false);
}

void UHorrorSettingsLocal::AddOrUpdateCustomKeyboardBindings(const FName MappingName, const FKey NewKey, const ULocalPlayer* LocalPlayer)
{
	if (MappingName == NAME_None) return;

	if (CustomKeyboardConfig.Find(MappingName))
	{
		CustomKeyboardConfig[MappingName] = NewKey;
	}
	else
	{
		CustomKeyboardConfig.Add(MappingName, NewKey);
	}

	if (auto* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
	{
		Subsystem->AddPlayerMappedKey(MappingName, NewKey);
	}
}

void UHorrorSettingsLocal::AddInputSetup(const FInputMappingSetup& InputSetup, UEnhancedInputLocalPlayerSubsystem* InputSubsystem)
{
	ensure(InputSubsystem);

	FModifyContextOptions Options = FModifyContextOptions();
	Options.bIgnoreAllPressedKeysUntilRelease = true;

	const auto* AssetManager = UHorrorAssetManager::Get();
	ensure(AssetManager);
	if (!AssetManager) return;

	const auto* MappingConfig = Cast<UPlayerMappableInputConfig>(AssetManager->LoadAssetSync(InputSetup.ConfigPath));
	ensure(MappingConfig);
	if (!MappingConfig) return;

	const int32 ExistingConfigIdx = LoadedInputConfigs.IndexOfByPredicate([&MappingConfig](const FLoadedInputMappingSetup& Pair) { return Pair.Config == MappingConfig; });

	if (ExistingConfigIdx == INDEX_NONE)
	{
		LoadedInputConfigs.Add(FLoadedInputMappingSetup(MappingConfig, InputSetup.Type));
	}
	InputSubsystem->AddPlayerMappableConfig(MappingConfig, Options);

	// Перегружаем назначенные пользователем клавиши управления
	for (const TPair<FName, FKey>& Pair : CustomKeyboardConfig)
	{
		if (Pair.Key != NAME_None && Pair.Value.IsValid())
		{
			InputSubsystem->AddPlayerMappedKey(Pair.Key, Pair.Value);
		}
	}
}

void UHorrorSettingsLocal::GetInputCategories(const APlayerController* PlayerController, ECommonInputType Type, TArray<FHorrorInputCategory>& Categories)
{
	const auto* GameSettings = Get();
	ensure(GameSettings);
	if (!GameSettings) return;

	const UPlayerMappableInputConfig* Config = nullptr;
	/** Поиск нужного конфига */
	{
		ensure(PlayerController);
		if (!PlayerController) return;

		const ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
		ensure(LocalPlayer);
		if (!LocalPlayer) return;

		const auto* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);
		ensure(Subsystem);
		if (!Subsystem) return;

		for (const auto& Setup : GameSettings->LoadedInputConfigs)
		{
			if (Setup.Type != Type) continue;
			Config = Setup.Config;
			break;
		}
	}
	if (!Config) return;

	for (const auto& KeyMap : Config->GetPlayerMappableKeys())
	{
		if (!KeyMap.bIsPlayerMappable) continue;

		FText CategoryName = KeyMap.PlayerMappableOptions.DisplayCategory;
		FHorrorInputCategory* Category = Categories.FindByPredicate([&CategoryName](const FHorrorInputCategory& Cat) { return Cat.DisplayName.EqualToCaseIgnored(CategoryName); });
		if (!Category)
		{
			FHorrorInputCategory NewCategory = FHorrorInputCategory(CategoryName);
			const int32 NewIndex = Categories.Add(NewCategory);
			Category = &Categories[NewIndex];
		}

		FHorrorInputBinding NewBinding = FHorrorInputBinding(KeyMap.PlayerMappableOptions.Name, KeyMap.PlayerMappableOptions.DisplayName, KeyMap.Key);
		if (auto* CustomKey = GameSettings->CustomKeyboardConfig.Find(KeyMap.PlayerMappableOptions.Name))
		{
			// Если задана кастомная клавиша - используем ее
			NewBinding.Key = *CustomKey;
		}
		Category->Bindings.Add(NewBinding);
	}
}

void UHorrorSettingsLocal::GetActionMappedKey(FName ActionName, bool& bSuccess, FKey& Key, FText& KeyName)
{
	bSuccess = false;

	const auto* GameSettings = Get();
	ensure(GameSettings);
	if (!GameSettings) return;

	if (auto* CustomKey = GameSettings->CustomKeyboardConfig.Find(ActionName))
	{
		bSuccess = true;
		Key = *CustomKey;
		KeyName = Key.GetDisplayName();
		return;
	}

	for (const auto& Setup : GameSettings->LoadedInputConfigs)
	{
		for (const auto& KeyMap : Setup.Config->GetPlayerMappableKeys())
		{
			if (!KeyMap.bIsPlayerMappable || KeyMap.PlayerMappableOptions.Name != ActionName) continue;

			bSuccess = true;
			Key = KeyMap.Key;
			KeyName = Key.GetDisplayName();
			return;
		}
	}
}

void UHorrorSettingsLocal::ApplyNonResolutionSettings()
{
	Super::ApplyNonResolutionSettings();

	// Check if Control Bus Mix references have been loaded,
	// Might be false if applying non resolution settings without touching any of the setters from UI
	if (!bSoundControlBusMixLoaded)
	{
		LoadAudioControlBusses();
	}
	if (OverallControlBus)
	{
		SetVolumeForControlBus(OverallControlBus, AudioVolume_Overall);
	}
	if (MusicControlBus)
	{
		SetVolumeForControlBus(MusicControlBus, AudioVolume_Music);
	}
	if (SoundFXControlBus)
	{
		SetVolumeForControlBus(SoundFXControlBus, AudioVolume_FX);
	}
}

void UHorrorSettingsLocal::LoadAudioControlBusses()
{
	if (!GEngine) return;
	const UWorld* World = GEngine->GetCurrentPlayWorld();
	if (!World) return;
	const auto* GameAudioSettings = GetDefault<UHorrorGameAudioSettings>();
	if (!GameAudioSettings) return;

	if (UObject* ObjPath = GameAudioSettings->OverallVolumeControlBus.TryLoad())
	{
		if (USoundControlBus* SoundControlBus = Cast<USoundControlBus>(ObjPath))
		{
			OverallControlBus = SoundControlBus;
		}
		else
		{
			ensureMsgf(SoundControlBus, TEXT("Overall Control Bus reference missing from SpiderGame Audio Settings."));
		}
	}

	if (UObject* ObjPath = GameAudioSettings->MusicVolumeControlBus.TryLoad())
	{
		if (auto* SoundControlBus = Cast<USoundControlBus>(ObjPath))
		{
			MusicControlBus = SoundControlBus;
		}
		else
		{
			ensureMsgf(SoundControlBus, TEXT("Music Control Bus reference missing from SpiderGame Audio Settings."));
		}
	}

	if (UObject* ObjPath = GameAudioSettings->FXVolumeControlBus.TryLoad())
	{
		if (USoundControlBus* SoundControlBus = Cast<USoundControlBus>(ObjPath))
		{
			SoundFXControlBus = SoundControlBus;
		}
		else
		{
			ensureMsgf(SoundControlBus, TEXT("SoundFX Control Bus reference missing from SpiderGame Audio Settings."));
		}
	}

	if (UObject* ObjPath = GameAudioSettings->UserSettingsControlBusMix.TryLoad())
	{
		if (USoundControlBusMix* SoundControlBusMix = Cast<USoundControlBusMix>(ObjPath))
		{
			ControlBusMix = SoundControlBusMix;

			const FSoundControlBusMixStage OverallControlBusMixStage = UAudioModulationStatics::CreateBusMixStage(World, OverallControlBus, AudioVolume_Overall);
			const FSoundControlBusMixStage MusicControlBusMixStage = UAudioModulationStatics::CreateBusMixStage(World, MusicControlBus, AudioVolume_Music);
			const FSoundControlBusMixStage SoundFXControlBusMixStage = UAudioModulationStatics::CreateBusMixStage(World, SoundFXControlBus, AudioVolume_FX);

			TArray<FSoundControlBusMixStage> ControlBusMixStageArray;
			ControlBusMixStageArray.Add(OverallControlBusMixStage);
			ControlBusMixStageArray.Add(MusicControlBusMixStage);
			ControlBusMixStageArray.Add(SoundFXControlBusMixStage);

			UAudioModulationStatics::UpdateMix(World, ControlBusMix, ControlBusMixStageArray);

			bSoundControlBusMixLoaded = true;
		}
		else
		{
			ensureMsgf(SoundControlBusMix, TEXT("User Settings Control Bus Mix reference missing from SpiderGame Audio Settings."));
		}
	}
}

void UHorrorSettingsLocal::SetVolumeForControlBus(USoundControlBus* InSoundControlBus, float InVolume) {}
