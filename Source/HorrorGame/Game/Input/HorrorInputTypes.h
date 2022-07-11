// It is owned by the company Dverg Verksted.

#pragma once

#include "CoreMinimal.h"
#include "CommonInputBaseTypes.h"
#include "HorrorInputTypes.generated.h"

class UPlayerMappableInputConfig;

/** Данные загруженных привязок клавиш */
USTRUCT(BlueprintType)
struct FLoadedInputMappingSetup
{
	GENERATED_BODY()

	FLoadedInputMappingSetup() = default;

	FLoadedInputMappingSetup(const UPlayerMappableInputConfig* InConfig, ECommonInputType InType) : Config(InConfig), Type(InType) {}

	/** Конфиг привязок */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	const UPlayerMappableInputConfig* Config = nullptr;

	/** Тип ввода к которому относится текущий конфиг */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	ECommonInputType Type = ECommonInputType::Count;
};

/** Настройки привязок клавиш */
USTRUCT(BlueprintType)
struct FInputMappingSetup
{
	GENERATED_BODY()

	FInputMappingSetup() = default;

	/** Путь к конфигу привязок */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ToolTip = "Конфиг. привязки клавиш", AllowedClasses = "PlayerMappableInputConfig"))
	FSoftObjectPath ConfigPath;

	/** Тип ввода к которому относится текущий конфиг */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECommonInputType Type = ECommonInputType::Count;
};

/** Привязка клавиш к действию */
USTRUCT(BlueprintType)
struct HORRORGAME_API FHorrorInputBinding
{
	GENERATED_USTRUCT_BODY()

	FHorrorInputBinding() = default;

	FHorrorInputBinding(const FName& InActionName, const FText& InDisplayName, const FKey& InKey) : ActionName(InActionName), DisplayName(InDisplayName), Key(InKey) {}

	/** Имя действия */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ActionName;

	/** Наименование действия */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText DisplayName;

	/** Текущая клавиша */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FKey Key;
};

/** Категория привязки клавиш */
USTRUCT(BlueprintType)
struct HORRORGAME_API FHorrorInputCategory
{
	GENERATED_USTRUCT_BODY()

	FHorrorInputCategory() = default;

	FHorrorInputCategory(const FText& InDisplayName) : DisplayName(InDisplayName) {}

	/** Наименование категории */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText DisplayName;

	/** Привязки клавиш */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FHorrorInputBinding> Bindings;
};
