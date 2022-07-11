// It is owned by the company Dverg Verksted.

#include "Game/InteractionSystem/DataAssets/DrawerDataAsset.h"

UDrawerDataAsset::UDrawerDataAsset() {}

FPrimaryAssetId UDrawerDataAsset::GetPrimaryAssetId() const
{
	return FPrimaryAssetId("Drawer", GetFName());
}
