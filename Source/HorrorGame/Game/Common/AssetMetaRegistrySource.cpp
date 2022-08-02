// It is owned by the company Dverg Verksted.

#include "Game/Common/AssetMetaRegistrySource.h"
#include "DataRegistrySettings.h"
#include "DataRegistrySubsystem.h"
#include "Engine/AssetManager.h"

void UAssetMetaRegistrySource::RefreshRuntimeSources()
{
	if (!IsInitialized()) return;

	UDataRegistrySubsystem* Subsystem = UDataRegistrySubsystem::Get();
	TArray<FAssetData> AssetDataList;
	const UScriptStruct* ItemStruct = GetItemStruct();

	if (!ItemStruct || !Subsystem) return;

	AssetsCache.Empty();

	if (EnumHasAnyFlags(AssetUsage, EMetaDataRegistrySourceAssetUsage::SearchAssets))
	{
		UAssetManager& AssetManager = UAssetManager::Get();
		FAssetManagerSearchRules TempRules = SearchRules;
		bool bExpandedVirtual = false;
		if (!TempRules.bSkipVirtualPathExpansion)
		{
			bExpandedVirtual = AssetManager.ExpandVirtualPaths(TempRules.AssetScanPaths);
			TempRules.bSkipVirtualPathExpansion = true;
		}

		AssetManager.SearchAssetRegistryPaths(AssetDataList, SearchRules);

		if (bExpandedVirtual)
		{
			// Need to register for changes to virtual paths
			if (!NewAssetSearchRootHandle.IsValid())
			{
				NewAssetSearchRootHandle = AssetManager.Register_OnAddedAssetSearchRoot(FOnAddedAssetSearchRoot::FDelegate::CreateUObject(this, &UAssetMetaRegistrySource::OnNewAssetSearchRoot));
			}
		}
	}

	if (EnumHasAnyFlags(AssetUsage, EMetaDataRegistrySourceAssetUsage::RegisterAssets))
	{
		for (const FRegisteredAsset& RegisteredPair : SpecificRegisteredAssets)
		{
			AssetDataList.Add(RegisteredPair.Key);
		}
	}

	for (FAssetData& AssetData : AssetDataList)
	{
		if (DoesAssetPassFilter(AssetData, false))
		{
			FName AssetUniqueName = NAME_None;
			if (AssetData.GetTagValue(AssetUniqueNameTag, AssetUniqueName) && AssetUniqueName != NAME_None)
			{
				FAssetMetaRegistryRow AssetCache;
				AssetCache.AssetID = AssetData.GetPrimaryAssetId();
				AssetCache.AssetPath = FSoftObjectPath(AssetData.ObjectPath);
				AssetsCache.Add(AssetUniqueName, AssetCache);
			}
		}
	}
}

bool UAssetMetaRegistrySource::IsAssetWithNameExists(const FName& Path, const FName& CheckName, FName& ExistsPath)
{
	bool Result = false;
	const UAssetManager& AssetManager = UAssetManager::Get();
	const IAssetRegistry& AssetRegistry = AssetManager.GetAssetRegistry();
	AssetRegistry.EnumerateAllAssets(
		[CheckName, Path, &Result, &ExistsPath](const FAssetData& AssetData)
		{
			FName AssetName = NAME_None;
			if (AssetData.GetTagValue(AssetUniqueNameTag, AssetName) && AssetName != NAME_None)
			{
				if (AssetData.ObjectPath != Path && AssetName == CheckName)
				{
					ExistsPath = AssetData.ObjectPath;
					Result = true;
					return false;
				}
			}
			return true;
		},
		true);
	return Result;
}

void UAssetMetaRegistrySource::OnNewAssetSearchRoot(const FString& SearchRoot)
{
	// By default tell our registry to refresh
	if (IsInitialized())
	{
		GetRegistry()->MarkRuntimeDirty();
		GetRegistry()->RuntimeRefreshIfNeeded();
	}
}

void UAssetMetaRegistrySource::GetResolvedNames(TArray<FName>& Names) const
{
	LastAccessTime = UDataRegistry::GetCurrentTime();
	AssetsCache.GetKeys(Names);
}

EDataRegistryAvailability UAssetMetaRegistrySource::GetSourceAvailability() const
{
	return EDataRegistryAvailability::PreCached;
}

EDataRegistryAvailability UAssetMetaRegistrySource::GetItemAvailability(const FName& ResolvedName, const uint8** PrecachedDataPtr) const
{
	LastAccessTime = UDataRegistry::GetCurrentTime();

	if (auto* Asset = AssetsCache.Find(ResolvedName))
	{
		*PrecachedDataPtr = (uint8*)Asset;
	}

	return EDataRegistryAvailability::PreCached;
}

FString UAssetMetaRegistrySource::GetDebugString() const
{
	if (const UDataRegistry* Registry = GetRegistry())
	{
		return FString::Printf(TEXT("Assets:%d SI:(%d)"), AssetsCache.Num(), Registry->GetSourceIndex(this));
	}
	return Super::GetDebugString();
}

bool UAssetMetaRegistrySource::AcquireItem(FDataRegistrySourceAcquireRequest&& Request)
{
	LastAccessTime = UDataRegistry::GetCurrentTime();

	PendingAcquires.Add(Request);
	// Tell it to go next frame
	FStreamableHandle::ExecuteDelegate(FStreamableDelegate::CreateUObject(this, &UAssetMetaRegistrySource::HandlePendingAcquires));

	return true;
}

bool UAssetMetaRegistrySource::DoesAssetPassFilter(const FAssetData& AssetData, bool bNewRegisteredAsset)
{
	const UDataRegistrySettings* Settings = GetDefault<UDataRegistrySettings>();

	// Call into parent to check search rules if needed
	if (bNewRegisteredAsset)
	{
		FAssetManagerSearchRules ModifiedRules = SearchRules;
		if (!UAssetManager::Get().DoesAssetMatchSearchRules(AssetData, ModifiedRules))
		{
			return false;
		}
	}

	FString AssetNameString;
	FName NoneName = NAME_None;

	if (AssetData.GetTagValue(AssetUniqueNameTag, AssetNameString) && AssetNameString != NoneName.ToString())
	{
		if (const auto* AssetClass = AssetData.GetClass())
		{
			if (AssetClass->IsChildOf(SearchRules.AssetBaseClass)) return true;
		}
	}

	return false;
}

void UAssetMetaRegistrySource::HandlePendingAcquires()
{
	LastAccessTime = UDataRegistry::GetCurrentTime();

	// Iterate manually to deal with recursive adds
	const int32 NumRequests = PendingAcquires.Num();
	for (int32 i = 0; i < NumRequests; i++)
	{
		// Make a copy in case array changes
		FDataRegistrySourceAcquireRequest Request = PendingAcquires[i];

		uint8 Sourceindex = 255;
		FName ResolvedName;

		if (Request.Lookup.GetEntry(Sourceindex, ResolvedName, Request.LookupIndex))
		{
			if (const auto* Asset = AssetsCache.Find(ResolvedName))
			{
				const UScriptStruct* ItemStruct = GetItemStruct();
				if (ensure(ItemStruct && ItemStruct->GetStructureSize()))
				{
					// Allocate new copy of struct, will be handed off to cache
					uint8* ItemStructMemory = AllocateItemMemory(ItemStruct);
					ItemStruct->CopyScriptStruct(ItemStructMemory, Asset);
					HandleAcquireResult(Request, EDataRegistryAcquireStatus::InitialAcquireFinished, ItemStructMemory);
					continue;
				}
			}
		}

		// Acquire failed for some reason, report failure for each one
		HandleAcquireResult(Request, EDataRegistryAcquireStatus::AcquireError, nullptr);
	}

	PendingAcquires.RemoveAt(0, NumRequests);
}

uint8* UAssetMetaRegistrySource::AllocateItemMemory(const UScriptStruct* ItemStruct)
{
	check(ItemStruct);
	uint8* ItemStructMemory = static_cast<uint8*>(FMemory::Malloc(ItemStruct->GetStructureSize()));
	ItemStruct->InitializeStruct(ItemStructMemory);
	return ItemStructMemory;
}
