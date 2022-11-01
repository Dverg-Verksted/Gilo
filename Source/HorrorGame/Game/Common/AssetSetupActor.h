// It is owned by the company Dverg Verksted.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AssetSetupActor.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UAssetSetupActor : public UInterface
{
	GENERATED_BODY()
};

/**
 * Интерфейс акторов, инициализируемых с помощью DataAsset
 */
class HORRORGAME_API IAssetSetupActor
{
	GENERATED_BODY()

public:
	/* Инициализация актора из DataAsset-а */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void InitFromAsset(const UPrimaryDataAsset* SourceAsset);
};
