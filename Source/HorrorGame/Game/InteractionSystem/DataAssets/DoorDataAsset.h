﻿// It is owned by the company Dverg Verksted.

#pragma once

#include "CoreMinimal.h"
#include "Game/Common/AutoDiscoveryAsset.h"
#include "DoorDataAsset.generated.h"

/**
 * Обычная дверь
 */
UCLASS()
class HORRORGAME_API UDoorDataAsset : public UAutoDiscoveryAsset
{
	GENERATED_BODY()

public:
	UDoorDataAsset();

	/* Модель двери */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Door", DisplayName = "Модель", AssetRegistrySearchable, meta = (ToolTip = "Модель собственно двери", AssetBundles = "meshes"))
	TSoftObjectPtr<UStaticMesh> DoorMesh;

	/* Минимальный угол открытия двери */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Door", DisplayName = "Мин. угол", meta = (ToolTip = "Мин. угол открытия двери"))
	float MinDoorAngle = 0.0f;

	/* Максимальный угол открытия двери */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Door", DisplayName = "Макс. угол", meta = (ToolTip = "Макс. угол открытия двери"))
	float MaxDoorAngle = 90.0f;

	/* Разрешается открытие с помощью движения мыши/контроллера */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Door", DisplayName = "Ручное открытие", meta = (ToolTip = "Разрешается открытие с помощью движения мыши/контроллера"))
	bool bDragEnabled = true;

	/* Тяжесть открытия */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Door", DisplayName = "Тяжесть открытия", meta = (ToolTip = "Тяжесть открытия. Чем меньше - тем тяжелее", EditCondition = "bDragEnabled"))
	float DragMagnitude = 10.0f;

	/* Скорость быстрого открытия/закрытия двери */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Door", DisplayName = "Скорость открытия", meta = (ToolTip = "Скорость быстрого открытия/закрытия двери. Чем меньше, тем медленнее"))
	float QuickOpenSpeed = 3.0f;

	/* Угол поворота открытой двери в автоматическом режиме */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Door", DisplayName = "Угол открытой двери", meta = (ToolTip = "Угол поворота открытой двери в автоматическом режиме"))
	float DoorOpenedAngle = 0.0f;

	/* Угол поворота закрытой двери в автоматическом режиме */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Door", DisplayName = "Угол закрытой двери", meta = (ToolTip = "Угол поворота закрытой двери в автоматическом режиме"))
	float DoorClosedAngle = 90.0f;

protected:
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
};
