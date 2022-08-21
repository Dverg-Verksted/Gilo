// It is owned by the company Dverg Verksted.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "GameBaseUILayer.generated.h"

/**
 * Основной слой UI
 */
UCLASS(Blueprintable, BlueprintType, Abstract, meta = (DisableNativeTick))
class HORRORGAME_API UGameBaseUILayer : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	UGameBaseUILayer();

	/* Проверка на активность любого меню
	 * Success - TRUE - Если меню активно. ActiveWidget - Виджет активного меню */
	UFUNCTION(BlueprintImplementableEvent, BlueprintPure)
	void IsAnyMenuActivated(bool& Success, UCommonActivatableWidget*& ActiveWidget);

	/* Добавить меню в стек. Возвращает созданный виджет */
	UFUNCTION(BlueprintImplementableEvent)
	void PushMenu(TSubclassOf<UCommonActivatableWidget> MenuClass, UCommonActivatableWidget*& Widget);

	/* Добавить диалог в стек. Возвращает созданный виджет */
	UFUNCTION(BlueprintImplementableEvent)
	void PushPrompt(TSubclassOf<UCommonActivatableWidget> PromptClass, UCommonActivatableWidget*& PromptWidget);
};
