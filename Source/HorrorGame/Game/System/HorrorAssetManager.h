// It is owned by the company Dverg Verksted.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "HorrorAssetManager.generated.h"

/**
 * Кастомный менеджер ассетов для проекта
 */
UCLASS()
class HORRORGAME_API UHorrorAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:
	UHorrorAssetManager();

	/* Возвращает Singletone объект Ассет Менеджера */
	static UHorrorAssetManager* Get();

	/** Синхронная загрузка ассета
	 * @param AssetPath Путь к ассету
	 * @return nullptr Если путь пустой или ассет не удалось загрузить
	 */
	UObject* LoadAssetSync(const FSoftObjectPath& AssetPath) const;

protected:
	/** Проверка на необходимость логирования времени загрузки ассетов
	 *@return TRUE - Если необходимо логировать время загрузки */
	static bool ShouldLogAssetLoads();
};
