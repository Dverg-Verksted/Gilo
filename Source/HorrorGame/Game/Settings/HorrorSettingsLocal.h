// It is owned by the company Dverg Verksted.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "EnhancedInputSubsystems.h"
#include "PlayerMappableInputConfig.h"
#include "Game/Input/HorrorInputTypes.h"
#include "HorrorSettingsLocal.generated.h"

class USoundControlBus;
class USoundControlBusMix;

/**
 * Настройки игры, которые хранятся локально на машине пользователя в файле конфигурации
 */
UCLASS()
class HORRORGAME_API UHorrorSettingsLocal : public UGameUserSettings
{
	GENERATED_BODY()

public:
	UHorrorSettingsLocal();

	/** Возвращает текущий экземпляр настроек игры */
	static UHorrorSettingsLocal* Get();

	UFUNCTION(BlueprintPure, Category = "GameSettings|Audio")
	float GetOverallVolume() const { return AudioVolume_Overall; }

	UFUNCTION(BlueprintCallable, Category = "GameSettings|Audio")
	void SetOverallVolume(float InVolume);

	UFUNCTION(BlueprintPure, Category = "GameSettings|Audio")
	float GetMusicVolume() const;

	UFUNCTION(BlueprintCallable, Category = "GameSettings|Audio")
	void SetMusicVolume(float InVolume);

	UFUNCTION(BlueprintPure, Category = "GameSettings|Audio")
	float GetFXVolume() const { return AudioVolume_FX; }

	UFUNCTION(BlueprintCallable, Category = "GameSettings|Audio")
	void SetFXVolume(float InVolume);

	/** Обновление привязки клавиши клавиатуры к действию
	 * @param PlayerController Контроллер игрока
	 * @param MappingName Имя действия
	 * @param NewKey Новая клавиша
	 * @param bSaveSettings TRUE - Если нужно сохранить настройки. False если только перегрузить клавишу
	 */
	UFUNCTION(BlueprintCallable, Category = "Settings|Input")
	static void UpdateKeyboardBinding(APlayerController* PlayerController, const FName MappingName, const FKey NewKey, bool bSaveSettings = true);

	/** Сохранение пользовательских настроек в файле конфигурации */
	UFUNCTION(BlueprintCallable, Category = "Settings")
	static void SaveUserSettings();

	/** Добавление новой или обновление существующей пользовательской привязки клавиши клавиатуры
	 *
	 * @param MappingName	Имя пользовательской привязки (настраивается в контексте ввода)
	 * @param NewKey		Клавиша клавиатуры
	 */
	void AddOrUpdateCustomKeyboardBindings(const FName MappingName, const FKey NewKey, const ULocalPlayer* LocalPlayer);

	/** Возвращает словарь переопределения клавиш отвечающих за действия */
	const TMap<FName, FKey>& GetCustomPlayerInputConfig() const { return CustomKeyboardConfig; }

	/** Добавляет новую настройку управления
	 * @param InputSetup Настройка управления
	 * @param InputSubsystem Подсистема ввода
	 */
	void AddInputSetup(const FInputMappingSetup& InputSetup, UEnhancedInputLocalPlayerSubsystem* InputSubsystem);

	/** Возвращает коллекцию категорий привязок клавиш */
	UFUNCTION(BlueprintCallable, Category = "Settings|Input")
	static void GetInputCategories(const APlayerController* PlayerController, ECommonInputType Type, TArray<FHorrorInputCategory>& Categories);

	/** Возвращает клавишу, привязанную к заданному действию
	 * @param ActionName Имя действия
	 * @param bSuccess TRUE - Если к действию привязана клавиша
	 * @param Key Привязанная клавиша
	 * @param KeyName Наименование клавиши
	 */
	UFUNCTION(BlueprintPure, Category = "Settings|Input")
	static void GetActionMappedKey(FName ActionName, bool& bSuccess, FKey& Key, FText& KeyName);

	virtual void ApplyNonResolutionSettings() override;

private:
	/** Коллекция загруженных конфигурация привязок клавиш */
	UPROPERTY(VisibleAnywhere)
	TArray<FLoadedInputMappingSetup> LoadedInputConfigs;

	/** Коллекция пользовательских привязок клавиш клавиатуры. Имя действия-Клавиша клавиатуры */
	UPROPERTY(Config)
	TMap<FName, FKey> CustomKeyboardConfig;

	/* Общий уровень громкости */
	UPROPERTY(Config)
	float AudioVolume_Overall = 1.0f;
	/* Уровень громкости музыки */
	UPROPERTY(Config)
	float AudioVolume_Music = 1.0f;
	/* Уровень громкости звуковых эффектов */
	UPROPERTY(Config)
	float AudioVolume_FX = 1.0f;

	UPROPERTY(Transient)
	USoundControlBusMix* ControlBusMix = nullptr;
	UPROPERTY(Transient)
	USoundControlBus* OverallControlBus = nullptr;
	UPROPERTY(Transient)
	USoundControlBus* MusicControlBus = nullptr;
	UPROPERTY(Transient)
	USoundControlBus* SoundFXControlBus = nullptr;
	bool bSoundControlBusMixLoaded = false;

	/* Загрузка шин управления звуками */
	void LoadAudioControlBusses();
	/* Установка уровня громкости для шины */
	void SetVolumeForControlBus(USoundControlBus* InSoundControlBus, float InVolume);
};
