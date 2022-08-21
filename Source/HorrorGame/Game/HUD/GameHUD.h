// It is owned by the company Dverg Verksted.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "Game/HUD/DebugHUD.h"
#include "GameHUD.generated.h"

/**
 * Базовый класс HUD
 */
UCLASS(Blueprintable, BlueprintType, Abstract)
class HORRORGAME_API AGameHUD : public ADebugHUD
{
	GENERATED_BODY()

public:
	AGameHUD(const FObjectInitializer& ObjectInitializer);

	/* Вызов игрового меню */
	UFUNCTION(BlueprintCallable, Category = "UI")
	void ToggleGameMenu();

	/* Добавить меню в стек. Возвращает созданный виджет */
	UFUNCTION(BlueprintCallable, Category = "UI")
	void PushMenu(TSubclassOf<UCommonActivatableWidget> MenuClass, UCommonActivatableWidget*& Widget);

	/* Добавить диалог в стек. Возвращает созданный виджет */
	UFUNCTION(BlueprintCallable, Category = "UI")
	void PushPrompt(TSubclassOf<UCommonActivatableWidget> PromptClass, UCommonActivatableWidget*& PromptWidget);

protected:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UI")
	TObjectPtr<class UGameBaseUILayer> BaseUILayer;

	/* Класс основного слоя UI */
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UGameBaseUILayer> BaseLayerClass;

	/* Класс внутри игрового меню */
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UCommonActivatableWidget> GameMenuClass;

	virtual void BeginPlay() override;

	// Draw the HUD.
	virtual void DrawHUD() override;

private:
	void CreateDefaultWidget();
};
