// It is owned by the company Dverg Verksted.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "GenericPromptBase.generated.h"

/** Настройки диалога */
USTRUCT(BlueprintType)
struct HORRORGAME_API FPromptSetup
{
	GENERATED_USTRUCT_BODY()

	FPromptSetup() : Timeout(0) {}

	/** Сообщение */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Caption;

	/** Текст подтверждения */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText ConfirmText;

	/** Текст отмены */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText CancelText;

	/** Если > 0 то по-истечении срока диалог закроется с отменой */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Timeout;
};

/**
 * Базовый класс диалога подтверждения действия
 */
UCLASS(BlueprintType, Blueprintable, Abstract, meta = (DisableNativeTick))
class HORRORGAME_API UGenericPromptBase : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	UGenericPromptBase();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPromptResultHandle);

	UPROPERTY(BlueprintAssignable)
	FOnPromptResultHandle OnPromptConfirm;

	UPROPERTY(BlueprintAssignable)
	FOnPromptResultHandle OnPromptCancel;

	UPROPERTY(BlueprintAssignable)
	FOnPromptResultHandle OnPromptTimeout;

	UFUNCTION(BlueprintNativeEvent)
	void InitPrompt(const FPromptSetup& Setup);

protected:
	void ClearTimer();
	/* Подтверждение диалога */
	UFUNCTION(BlueprintCallable, Category = "UI")
	void ConfirmPrompt();

	/* Отмена диалога */
	UFUNCTION(BlueprintCallable, Category = "UI")
	void CancelPrompt();

	UFUNCTION()
	void OnTimeoutHandler();

private:
	/* Timeout timer handle */
	FTimerHandle TimeoutTimerHandle;
};
