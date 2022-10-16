// It is owned by the company Dverg Verksted.

#include "Game/BotAI/DataAssets/BotDataAssetBase.h"
#include "Game/Common/AssetMetaRegistrySource.h"

UBotDataAssetBase::UBotDataAssetBase() {}

FPrimaryAssetId UBotDataAssetBase::GetPrimaryAssetId() const
{
	return FPrimaryAssetId("Bot", GetFName());
}
