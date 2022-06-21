// It is owned by the company Dverg Verksted.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractiveObject.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInteractiveObject : public UInterface
{
	GENERATED_BODY()
};

/**
 * Интерактивный объект
 */
class HORRORGAME_API IInteractiveObject
{
	GENERATED_BODY()

public:

	/* Наведение курсора на интерактивный объект */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="ObjectInteraction")
	void OnHoverBegin(APlayerController* PlayerController, const FHitResult& Hit);

	/* Убрали курсор с интерактивного объекта */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="ObjectInteraction")
	void OnHoverEnd(APlayerController* PlayerController);
};
