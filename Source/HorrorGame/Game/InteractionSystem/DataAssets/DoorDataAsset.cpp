// It is owned by the company Dverg Verksted.


#include "Game/InteractionSystem/DataAssets/DoorDataAsset.h"


#include "Game/Common/AssetMetaRegistrySource.h"


UDoorDataAsset::UDoorDataAsset()
{
}

FPrimaryAssetId UDoorDataAsset::GetPrimaryAssetId() const
{
	return FPrimaryAssetId("Door", GetFName());
}
