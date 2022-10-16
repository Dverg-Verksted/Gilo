// It is owned by the company Dverg Verksted.


#include "BotSettings.h"

UBotSettings::UBotSettings()
{
	WalkSpeed = 400.0f;
	RunSpeedParam = 1.5f;
	ISightRadius = 2000.0f;
	ISightAge = 2.0f;
	ILoseSightRadius = ISightRadius + 500.0f;
	IFieldOfView = 90.0f;
	bIsPlayerDetected = false;
	DistanceToPlayer = 0.0f;
}

