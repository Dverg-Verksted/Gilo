// It is owned by the company Dverg Verksted.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CommonGameSettings.generated.h"

/**
 * Общеигровые настройки
 */
UCLASS(Config = Game, defaultconfig, meta = (DisplayName = "Общеигровые настройки"))
class HORRORGAME_API UCommonGameSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UCommonGameSettings();

	/* Виджет общего экрана загрузки */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "LoadingScreen", DisplayName = "LoadingScreen",
		meta = (MetaClass = "UserWidget", ToolTip = "Виджет, который используется на общих экранах загрузки. Например при загрузке транзитного уровня"))
	FSoftClassPath LoadingScreenClass;

	/* Z-Order Загрузочного экрана*/
	UPROPERTY(config, EditAnywhere, Category = "LoadingScreen")
	int32 LoadingScreenZOrder = 10000;

	/* Время отображения загрузочного экрана, после команды на его закрытие
	 * Может использоваться для доп.времени на прогрузку текстур */
	UPROPERTY(config, EditAnywhere, Category = "LoadingScreen", meta = (ForceUnits = s, ConsoleVariable = "CommonLoadingScreen.HoldLoadingScreenAdditionalSecs"))
	float HoldLoadingScreenAdditionalSecs = 2.0f;

	/* TRUE - Если требуется дополнительное время отображения экрана загрузки в редакторе */
	UPROPERTY(Transient, EditAnywhere, Category = "LoadingScreen", AdvancedDisplay)
	bool HoldLoadingScreenAdditionalSecsEvenInEditor = false;

	static FORCEINLINE const UCommonGameSettings* Get()
	{
		const UCommonGameSettings* Settings = GetDefault<UCommonGameSettings>();
		ensure(Settings);
		return Settings;
	}
};
