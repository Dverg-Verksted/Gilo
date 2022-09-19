// It is owned by the company Dverg Verksted.

#include "Game/Player/PlayerSettings.h"

UPlayerSettings::UPlayerSettings()
{
	WalkSpeed = 400.0f;
	MaxWalkSpeedCrouched = 200.0f;
	RunSpeedMagnitude = 1.25f;
	StrafeMoveMagnitude = 0.75f;
	BackMoveMagnitude = 0.6f;
	MaxPeekSlowDown = 0.3f;

	PeekLeftOffset = FVector(0.0f, -15.0f, -12.0f);
	PeekRightOffset = FVector(0.0f, 15.0f, -12.0f);
	PeekRotation = 20.0f;

	MaxStaminaAmount = 15.0f;
	MinStaminaAmount = 3.75f;
	StaminaDrainRate = 1.0f;
	StaminaRestoreRate = 0.3f;
	JumpVelocity = 420.0f;
	MaxHealth = 100.0f;
	HealthRegenDelay = 7.0f;
}
