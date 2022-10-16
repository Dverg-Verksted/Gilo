// It is owned by the company Dverg Verksted.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "Game/Input/HorrorInputTypes.h"
#include "PlayerSettings.generated.h"

/**
 * Настройки игрока
 */
UCLASS(Config = Game, defaultconfig, meta = (DisplayName = "Настройки игрока"))
class HORRORGAME_API UPlayerSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPlayerSettings();

	/* Скорость ходьбы */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Перемещение", DisplayName = "Скорость ходьбы")
	float WalkSpeed;

	/* Макс. скорость ходьбы в приседе */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Перемещение", DisplayName = "Макс. скорость ходьбы присед")
	float MaxWalkSpeedCrouched;

	/* Модификатор скорость бега */
	UPROPERTY(
		Config, EditAnywhere, BlueprintReadOnly, Category = "Перемещение", DisplayName = "Коэф. ускорения бега", meta = (ToolTip = "Коэффициент ускорения при беге", ClampMin = 1.0f, UIMin = 1.0f))
	float RunSpeedMagnitude;

	/* Модификатор скорости движения влево-вправо */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Перемещение", DisplayName = "Коэф. ходьбы вбок",
		meta = (ToolTip = "Модификатор скорости движения влево-вправо", ClampMax = 1.0f, ClampMin = 0.1f, UIMax = 1.0f, UIMin = 0.1f))
	float StrafeMoveMagnitude;

	/* Модификатор скорости движения назад */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Перемещение", DisplayName = "Коэф. ходьбы назад",
		meta = (ToolTip = "Модификатор скорости движения назад", ClampMax = 1.0f, ClampMin = 0.1f, UIMax = 1.0f, UIMin = 0.1f))
	float BackMoveMagnitude;

	/* Модификатор скорости движения в наклоне */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Перемещение", DisplayName = "Замедление в наклоне",
		meta = (ToolTip = "Модификатор замедления в наклоне", ClampMax = 1.0f, ClampMin = 0.0f, UIMax = 1.0f, UIMin = 0.0f))
	float MaxPeekSlowDown;

	/* Смещение при наклоне влево */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Перемещение", DisplayName = "Смещение наклона влево", meta = (ToolTip = "Смещение камеры при наклоне влево"))
	FVector PeekLeftOffset;

	/* Смещение при наклоне вправо */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Перемещение", DisplayName = "Смещение наклона вправо", meta = (ToolTip = "Смещение камеры при наклоне вправо"))
	FVector PeekRightOffset;

	/* Угол поворота камеры в наклоне */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Перемещение", DisplayName = "Угол наклона камеры",
		meta = (ToolTip = "Угол поворота камеры в наклоне", ClampMax = 60.0f, ClampMin = 0.0f, UIMax = 60.0f, UIMin = 0.0f))
	float PeekRotation;

	/* Макс. запас выносливости */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Перемещение", DisplayName = "Макс. выносливость", meta = (ToolTip = "Максимальный запас выносливости"))
	float MaxStaminaAmount;

	/* Мин. запас выносливости для бега */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Перемещение", DisplayName = "Мин. выносливость", meta = (ToolTip = "Минимальный запас выносливости для возобновления бега"))
	float MinStaminaAmount;

	/* Темп расхода выносливости при беге */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Перемещение", DisplayName = "Расход выносливости", meta = (ToolTip = "Темп расхода выносливости при беге"))
	float StaminaDrainRate;

	/* Темп восстановления выносливости */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Перемещение", DisplayName = "Восстановление выносливости", meta = (ToolTip = "Темп восстановления выносливости"))
	float StaminaRestoreRate;

	/* Сила прыжка (вместе с гравитацией влияет на высоту) */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Перемещение", DisplayName = "Сила прыжка", meta = (ToolTip = "Сила прыжка (вместе с гравитацией влияет на высоту)"))
	float JumpVelocity;

	/** Настройки привязки клавишь для разных устройств ввода*/
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, AdvancedDisplay, Category = "Управление", meta = (ToolTip = "Настройки привязки клавиш"))
	TArray<FInputMappingSetup> InputSetups;

	/* Максимальный запас здоровья */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Здоровье", DisplayName = "Макс. запас здоровья", meta = (ToolTip = "Максимальный запас здоровья игрока"))
	float MaxHealth;

	/* Задержка полного восстановления здоровья */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Здоровье", DisplayName = "Задержка регенерации", meta = (ToolTip = "Время от урона до полного восстановления здоровья"))
	float HealthRegenDelay;

	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "AI")
	float ISightRadius;

	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "AI")
	float ISightAge;

	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "AI", meta = (ToolTip = "Радиус начала потери видимости", ClampMax = 4000.0f, ClampMin = 1500.0f, UIMax = 4000.0f, UIMin = 1500.0f))
	float ILoseSightRadius;

	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "AI")
	float IFieldOfView;

	static FORCEINLINE const UPlayerSettings* Get()
	{
		const UPlayerSettings* Settings = GetDefault<UPlayerSettings>();
		ensure(Settings);
		return Settings;
	}
};
