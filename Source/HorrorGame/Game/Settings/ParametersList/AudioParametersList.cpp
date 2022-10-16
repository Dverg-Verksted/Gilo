// It is owned by the company Dverg Verksted.

#include "AudioParametersList.h"
#include "Game/Settings/HorrorSettingsLocal.h"
#include "GameConfigModel/Parameters/GameParameter_FloatRange.h"

#define LOCTEXT_NAMESPACE "GameParameters"

UAudioParametersList::UAudioParametersList() {}

void UAudioParametersList::CreateMusicVolumeParam()
{
	auto* Param = Cast<UGameParameter_FloatRange>(CreateParameter(TEXT("MusicVolume"), UGameParameter_FloatRange::StaticClass()));
	if (!Param) return;
	Param->SetDisplayName(LOCTEXT("MusicVolume_Name", "Music Volume"));
	Param->SetDynamicGetter(GET_LOCAL_GAME_SETTINGS_FUNCTION_PATH(GetMusicVolume));
	Param->SetDynamicSetter(GET_LOCAL_GAME_SETTINGS_FUNCTION_PATH(SetMusicVolume));
	Param->SetMaxValue(1.0f);
	Param->SetMinValue(0.0f);
	Param->SetDefaultValue(1.0f);
	Param->Initialize(LocalPlayer);
}

void UAudioParametersList::CreateSFXVolumeParam()
{
	auto* Param = Cast<UGameParameter_FloatRange>(CreateParameter(TEXT("SFXVolume"), UGameParameter_FloatRange::StaticClass()));
	if (!Param) return;
	Param->SetDisplayName(LOCTEXT("SFXVolume_Name", "SFX Volume"));
	Param->SetDynamicGetter(GET_LOCAL_GAME_SETTINGS_FUNCTION_PATH(GetFXVolume));
	Param->SetDynamicSetter(GET_LOCAL_GAME_SETTINGS_FUNCTION_PATH(SetFXVolume));
	Param->SetMaxValue(1.0f);
	Param->SetMinValue(0.0f);
	Param->SetDefaultValue(1.0f);
	Param->Initialize(LocalPlayer);
}

void UAudioParametersList::Initialize(ULocalPlayer* InLocalPlayer)
{
	if (LocalPlayer == InLocalPlayer) return;
	LocalPlayer = InLocalPlayer;

	CreateMusicVolumeParam();
	CreateSFXVolumeParam();
}

#undef LOCTEXT_NAMESPACE