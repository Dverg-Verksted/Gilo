// It is owned by the company Dverg Verksted.

#pragma once

#include "CoreMinimal.h"
#include "LoadingScreenSubsystem.generated.h"

/**
 * Подсистема отображения экрана загрузки
 */
UCLASS()
class HORRORGAME_API ULoadingScreenSubsystem : public UGameInstanceSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

public:
	//~USubsystem interface
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	//~End of USubsystem interface

	//~FTickableObjectBase interface
	virtual void Tick(float DeltaTime) override;
	virtual ETickableTickType GetTickableTickType() const override;
	virtual bool IsTickable() const override;
	virtual TStatId GetStatId() const override;
	virtual UWorld* GetTickableGameObjectWorld() const override;
	//~End of FTickableObjectBase interface

private:
	/** Ссылка на текущий виджет экрана загрузки */
	TSharedPtr<SWidget> LoadingScreenWidget;
	/** Пустой обработчик ввода, поглощающий весь пользовательский ввод на время отображения виджета */
	TSharedPtr<IInputProcessor> InputPreProcessor;
	/* TRUE Если между PreLoadMap и PostLoadMap*/
	bool bLoadingMap = false;
	/* Отладочная причина отображения экрана загрузки */
	FString DebugReasonForShowingOrHidingLoadingScreen;
	/** The time the loading screen most recently wanted to be dismissed (might still be up due to a min display duration requirement) **/
	double TimeLoadingScreenLastDismissed = -1.0;
	/* TRUE - Если загрузочный экран отображается пользователю */
	bool bShowingLoadingScreen = false;
	/* Время, когда было запущено отображение загрузочного экрана */
	double TimeLoadingScreenShown = 0.0;

	/* Проверка на наличие ТРЕБОВАНИЯ! для отображения загрузочного экрана */
	bool CheckForAnyNeedToShowLoadingScreen();
	/* Возвращает TRUE Если требуется отображение загрузочного экрана */
	bool ShouldShowLoadingScreen();
	/* Возвращает TRUE Если отображается инициализационный загрузочный экран */
	static bool IsShowingInitialLoadingScreen();
	void ChangePerformanceSettings(bool bEnabingLoadingScreen);
	/* Показать загрузочный экран */
	void ShowLoadingScreen();
	/* Скрыть загрузочный экран */
	void HideLoadingScreen();
	/** Обновление состояния загрузочного экрана */
	void UpdateLoadingScreen();
	/** Removes the widget from the viewport */
	void RemoveWidgetFromViewport();

	/** Блокировка пользовательского ввода */
	void StartBlockingInput();
	/** Разблокировка пользовательского ввода */
	void StopBlockingInput();

	/* Обработчик начала загрузки мира */
	void PreLoadMapHandler(const FWorldContext& WorldContext, const FString& String);
	/* Обработчик завершения загрузки мира */
	void PostLoadMapHandler(UWorld* World);
};
