// It is owned by the company Dverg Verksted.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "EnhancedInputSubsystems.h"
#include "PlayerMappableInputConfig.h"
#include "HorrorSettingsLocal.generated.h"

/**
 * Настройки игры, которые хранятся локально на машине пользователя в файле конфигурации
 */
UCLASS()
class HORRORGAME_API UHorrorSettingsLocal : public UGameUserSettings
{
	GENERATED_BODY()

public:
	UHorrorSettingsLocal();

	static UHorrorSettingsLocal* Get();

	/** INPUT SETTINGS BEGIN */

	UFUNCTION(BlueprintCallable)
	static void UpdateKeyboardBinding(APlayerController* PlayerController, const FName MappingName, const FKey NewKey);

	/**
	 * Добавление новой или обновление существующей пользовательской привязки клавиши клавиатуры
	 *
	 * @param MappingName	Имя пользовательской привязки (настраивается в контексте ввода)
	 * @param NewKey		Клавиша клавиатуры
	 */
	void AddOrUpdateCustomKeyboardBindings(const FName MappingName, const FKey NewKey, ULocalPlayer* LocalPlayer);

	const TMap<FName, FKey>& GetCustomPlayerInputConfig() const { return CustomKeyboardConfig; }

	void AddDefaultMappings(const UPlayerMappableInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const;

	/** INPUT SETTINGS END */

private:
	/** Коллекция пользовательских привязок клавиш клавиатуры. Имя действия-Клавиша клавиатуры */
	UPROPERTY(Config)
	TMap<FName, FKey> CustomKeyboardConfig;
};
