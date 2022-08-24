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
	/* Инициализация интерактивного объекта из DataAsset-а */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ObjectInteraction")
	void InitFromAsset(UPrimaryDataAsset* SourceAsset);

	/* Наведение курсора на интерактивный объект */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ObjectInteraction")
	void OnHoverBegin(APlayerController* PlayerController, const FHitResult& Hit);

	/* Убрали курсор с интерактивного объекта */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ObjectInteraction")
	void OnHoverEnd(APlayerController* PlayerController);

	/* Использование предмета */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ObjectInteraction")
	void OnUseObject(APlayerController* PlayerController);

	/** Проверка можно ли схватить объект с помощью физики
	 * @param CanBeGrabbed TRUE - Если объект поддерживает захват физикой
	 * @param ComponentToGrab - Компонент, за который хватается объект */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ObjectInteraction|Physics")
	void CanBeGrabbed(bool& CanBeGrabbed, UPrimitiveComponent*& ComponentToGrab);

	/** Вызывается при захвате физикой */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ObjectInteraction|Physics")
	void OnObjectGrabbed();

	/** Вызывается при завершении захвата физикой */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ObjectInteraction|Physics")
	void OnObjectReleased();
};
