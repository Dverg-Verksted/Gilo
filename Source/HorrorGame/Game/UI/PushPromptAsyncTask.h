// It is owned by the company Dverg Verksted.

#pragma once

#include "CoreMinimal.h"
#include "Controls/GenericPromptBase.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "UObject/Object.h"
#include "PushPromptAsyncTask.generated.h"

/**
 * Асинхронный таск для отображения окна диалога
 */
UCLASS()
class HORRORGAME_API UPushPromptAsyncTask : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UPushPromptAsyncTask(const FObjectInitializer& ObjectInitializer);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPromptResultOutputPin);

	UPROPERTY(BlueprintAssignable)
	FOnPromptResultOutputPin Confirm;

	UPROPERTY(BlueprintAssignable)
	FOnPromptResultOutputPin Cancel;

	UPROPERTY(BlueprintAssignable)
	FOnPromptResultOutputPin Timeout;

	/* Отображает окно диалога */
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContext"), Category = "UI")
	static UPushPromptAsyncTask* ShowPrompt(UObject* WorldContext, APlayerController* PlayerController, TSubclassOf<UGenericPromptBase> Class, const FPromptSetup& Setup);

	/* Привязка к событиями диалога */
	void BindToPrompt();

	/* Отвязка от событий диалога */
	void UnbindFromPrompt();

	// UBlueprintAsyncActionBase interface
	virtual void Activate() override;
	// End of UBlueprintAsyncActionBase interface

	UFUNCTION()
	void OnConfirmHandler();

	UFUNCTION()
	void OnCancelHandler();

	UFUNCTION()
	void OnTimeoutHandler();

private:
	UPROPERTY()
	TObjectPtr<UObject> WorldContextObject;

	UPROPERTY()
	TWeakObjectPtr<APlayerController> PlayerControllerWeakPtr;

	UPROPERTY()
	TObjectPtr<UGenericPromptBase> PromptWidget;

	UPROPERTY()
	TSubclassOf<UGenericPromptBase> PromptClass;

	FPromptSetup PromptSetup;
};
