// It is owned by the company Dverg Verksted.

#include "CommonGameSettings.h"

UCommonGameSettings::UCommonGameSettings() {}

FName UCommonGameSettings::GetGameStartLevel()
{
	if (auto* Settings = Get())
	{
		return Settings->MainGameLevel;
	}
	return NAME_None;
}
