// It is owned by the company Dverg Verksted.

#pragma once

#include "CoreMinimal.h"
#include "DataRegistrySource.h"
#include "Engine/DataTable.h"
#include "AssetMetaRegistrySource.generated.h"

USTRUCT(BlueprintType, DisplayName = "Asset MetaData Registry Row")
struct HORRORGAME_API FAssetMetaRegistryRow : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	FAssetMetaRegistryRow() {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FPrimaryAssetId AssetID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSoftObjectPath AssetPath;
};

/**
 * Автоматически наполняемый источник данных для регистра
 * Ищет подходящие ассеты
 */
UCLASS(DisplayName = "Asset MetaData Source")
class HORRORGAME_API UAssetMetaRegistrySource : public UDataRegistrySource
{
	GENERATED_BODY()
public:
	/** Asset usage */
	UPROPERTY(EditAnywhere, Category = DataRegistry)
	EMetaDataRegistrySourceAssetUsage AssetUsage;

	/** Asset registry scan rules */
	UPROPERTY(EditAnywhere, Category = DataRegistry, Meta = (EditCondition = "AssetUsage != EMetaDataRegistrySourceAssetUsage::NoAssets"))
	FAssetManagerSearchRules SearchRules;

	inline static const FName AssetUniqueNameTag = "AssetUniqueName";

	static bool IsAssetWithNameExists(const FName& Path, const FName& CheckName, FName& ExistsPath);

protected:
	/** Callback registered when an asset search root as been added post launch */
	virtual void OnNewAssetSearchRoot(const FString& SearchRoot);

	/** Last time this was accessed */
	mutable float LastAccessTime;

	// Source interface
	virtual void RefreshRuntimeSources() override;
	virtual void GetResolvedNames(TArray<FName>& Names) const override;
	virtual EDataRegistryAvailability GetSourceAvailability() const override;
	virtual EDataRegistryAvailability GetItemAvailability(const FName& ResolvedName, const uint8** PrecachedDataPtr) const override;
	virtual FString GetDebugString() const override;
	virtual bool AcquireItem(FDataRegistrySourceAcquireRequest&& Request) override;

	/** Returns true if this asset data passes the filter for this meta source, bNewRegisteredAsset is true if it comes from a
	 * RegisterSpecificAsset call and needs extra path checking */
	virtual bool DoesAssetPassFilter(const FAssetData& AssetData, bool bNewRegisteredAsset);

	/** Tells it to go through each pending acquire */
	void HandlePendingAcquires();

	TMap<FName, FAssetMetaRegistryRow> AssetsCache;

	/** List of specific assets registered with source, in runtime order sorted by priority */
	typedef TPair<FAssetData, int32> FRegisteredAsset;
	TArray<FRegisteredAsset> SpecificRegisteredAssets;

	/** Delegate handle for OnNewAssetSearchRoot */
	FDelegateHandle NewAssetSearchRootHandle;

	/** List of requests to resolve when table is loaded */
	TArray<FDataRegistrySourceAcquireRequest> PendingAcquires;

private:
	static uint8* AllocateItemMemory(const UScriptStruct* ItemStruct);
};
