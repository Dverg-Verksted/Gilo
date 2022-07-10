// It is owned by the company Dverg Verksted.

#include "Game/Settings/HorrorSettingsLocal.h"
#include "EnhancedInputSubsystems.h"
#include "Game/Player/PlayerSettings.h"

UHorrorSettingsLocal::UHorrorSettingsLocal() {}

UHorrorSettingsLocal* UHorrorSettingsLocal::Get()
{
	return GEngine ? CastChecked<UHorrorSettingsLocal>(GEngine->GetGameUserSettings()) : nullptr;
}

void UHorrorSettingsLocal::UpdateKeyboardBinding(APlayerController* PlayerController, const FName MappingName, const FKey NewKey)
{
	ensure(PlayerController);
	if (!PlayerController) return;

	auto* GameSettings = UHorrorSettingsLocal::Get();
	ensure(GameSettings);
	if (!GameSettings) return;

	if (auto* LocalPlayer = PlayerController->GetLocalPlayer())
	{
		GameSettings->AddOrUpdateCustomKeyboardBindings(MappingName, NewKey, LocalPlayer);
	}
}

void UHorrorSettingsLocal::AddOrUpdateCustomKeyboardBindings(const FName MappingName, const FKey NewKey, ULocalPlayer* LocalPlayer)
{
	if (MappingName == NAME_None) return;

	if (FKey* ExistingMapping = CustomKeyboardConfig.Find(MappingName))
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

void UHorrorSettingsLocal::AddDefaultMappings(const UPlayerMappableInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
{
	ensure(InputConfig);
	ensure(InputSubsystem);

	ULocalPlayer* LocalPlayer = InputSubsystem->GetLocalPlayer();
	ensure(LocalPlayer);

	FModifyContextOptions Options = FModifyContextOptions();
	Options.bIgnoreAllPressedKeysUntilRelease = true;

	// Добавляем конфиг по-умолчанию
	InputSubsystem->AddPlayerMappableConfig(InputConfig, Options);

	// Перегружаем назначенные пользователем клавиши управления
	for (const TPair<FName, FKey>& Pair : CustomKeyboardConfig)
	{
		if (Pair.Key != NAME_None && Pair.Value.IsValid())
		{
			InputSubsystem->AddPlayerMappedKey(Pair.Key, Pair.Value);
		}
	}
}
