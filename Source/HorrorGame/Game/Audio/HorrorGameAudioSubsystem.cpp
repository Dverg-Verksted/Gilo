// It is owned by the company Dverg Verksted.

#include "HorrorGameAudioSubsystem.h"
#include "AudioModulationStatics.h"
#include "HorrorGameAudioSettings.h"
#include "Game/Settings/HorrorSettingsLocal.h"

void UHorrorGameAudioSubsystem::ApplyVolumeSettings(UObject* WorldContextObject)
{
	if (WorldContextObject)
	{
		UpdateUserMix(WorldContextObject->GetWorld());
	}
}

void UHorrorGameAudioSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UHorrorGameAudioSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

bool UHorrorGameAudioSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	bool bShouldCreateSubsystem = Super::ShouldCreateSubsystem(Outer);
	if (Outer)
	{
		if (const UWorld* World = Outer->GetWorld())
		{
			bShouldCreateSubsystem = DoesSupportWorldType(World->WorldType) && bShouldCreateSubsystem;
		}
	}
	return bShouldCreateSubsystem;
}

void UHorrorGameAudioSubsystem::PostInitialize()
{
	const auto* AudioSettings = GetDefault<UHorrorGameAudioSettings>();
	if (!AudioSettings) return;

	if (UObject* ObjPath = AudioSettings->DefaultControlBusMix.TryLoad())
	{
		if (USoundControlBusMix* SoundControlBusMix = Cast<USoundControlBusMix>(ObjPath))
		{
			DefaultBaseMix = SoundControlBusMix;
		}
		else
		{
			ensureMsgf(SoundControlBusMix, TEXT("Default Control Bus Mix reference missing from Lyra Audio Settings."));
		}
	}

	if (UObject* ObjPath = AudioSettings->UserSettingsControlBusMix.TryLoad())
	{
		if (USoundControlBusMix* SoundControlBusMix = Cast<USoundControlBusMix>(ObjPath))
		{
			UserMix = SoundControlBusMix;
		}
		else
		{
			ensureMsgf(SoundControlBusMix, TEXT("User Control Bus Mix reference missing from Lyra Audio Settings."));
		}
	}

	if (UObject* ObjPath = AudioSettings->OverallVolumeControlBus.TryLoad())
	{
		if (USoundControlBus* SoundControlBus = Cast<USoundControlBus>(ObjPath))
		{
			OverallControlBus = SoundControlBus;
		}
		else
		{
			ensureMsgf(SoundControlBus, TEXT("Overall Control Bus reference missing from Lyra Audio Settings."));
		}
	}

	if (UObject* ObjPath = AudioSettings->MusicVolumeControlBus.TryLoad())
	{
		if (USoundControlBus* SoundControlBus = Cast<USoundControlBus>(ObjPath))
		{
			MusicControlBus = SoundControlBus;
		}
		else
		{
			ensureMsgf(SoundControlBus, TEXT("Music Control Bus reference missing from Lyra Audio Settings."));
		}
	}

	if (UObject* ObjPath = AudioSettings->FXVolumeControlBus.TryLoad())
	{
		if (USoundControlBus* SoundControlBus = Cast<USoundControlBus>(ObjPath))
		{
			SoundFXControlBus = SoundControlBus;
		}
		else
		{
			ensureMsgf(SoundControlBus, TEXT("SoundFX Control Bus reference missing from Lyra Audio Settings."));
		}
	}
}

void UHorrorGameAudioSubsystem::UpdateUserMix(const UWorld* World)
{
	// Загружаем локальные настройки пользователя
	if (const auto* LocalSettings = UHorrorSettingsLocal::Get())
	{
		if (UserMix)
		{
			// Активируем пользовательский микс
			UAudioModulationStatics::ActivateBusMix(World, UserMix);

			if (OverallControlBus && MusicControlBus && SoundFXControlBus)
			{
				const FSoundControlBusMixStage OverallControlBusMixStage = UAudioModulationStatics::CreateBusMixStage(World, OverallControlBus, LocalSettings->GetOverallVolume());
				const FSoundControlBusMixStage MusicControlBusMixStage = UAudioModulationStatics::CreateBusMixStage(World, MusicControlBus, LocalSettings->GetMusicVolume());
				const FSoundControlBusMixStage SoundFXControlBusMixStage = UAudioModulationStatics::CreateBusMixStage(World, SoundFXControlBus, LocalSettings->GetFXVolume());

				TArray<FSoundControlBusMixStage> ControlBusMixStageArray;
				ControlBusMixStageArray.Add(OverallControlBusMixStage);
				ControlBusMixStageArray.Add(MusicControlBusMixStage);
				ControlBusMixStageArray.Add(SoundFXControlBusMixStage);

				UAudioModulationStatics::UpdateMix(World, UserMix, ControlBusMixStageArray);
			}
		}
	}
}

void UHorrorGameAudioSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	const UWorld* World = InWorld.GetWorld();
	if (!World) return;

	if (DefaultBaseMix)
	{
		// Активируем основной микс
		UAudioModulationStatics::ActivateBusMix(World, DefaultBaseMix);
	}
	UpdateUserMix(World);
}

bool UHorrorGameAudioSubsystem::DoesSupportWorldType(const EWorldType::Type WorldType) const
{
	return (WorldType == EWorldType::Game || WorldType == EWorldType::PIE);
}
