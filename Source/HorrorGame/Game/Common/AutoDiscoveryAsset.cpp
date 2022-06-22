// It is owned by the company Dverg Verksted.


#include "Game/Common/AutoDiscoveryAsset.h"

#include "AssetMetaRegistrySource.h"
#include "Misc/MessageDialog.h"

#define LOCTEXT_NAMESPACE "BuildingSystem"

void UAutoDiscoveryAsset::GetAssetRegistryTags(TArray<FAssetRegistryTag>& OutTags) const
{
	OutTags.Add(FAssetRegistryTag(UAssetMetaRegistrySource::AssetUniqueNameTag, AssetName.ToString(), FAssetRegistryTag::TT_Alphabetical));
	Super::GetAssetRegistryTags(OutTags);
}

void UAutoDiscoveryAsset::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	// Проверяем имя ассета на уникальность
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (PropertyChangedEvent.Property && PropertyChangedEvent.Property->GetFName() ==
		GET_MEMBER_NAME_CHECKED(UAutoDiscoveryAsset, AssetName))
	{
		const FName NewVal = AssetName;
		if (PropertyChangedEvent.ChangeType == EPropertyChangeType::ValueSet && NewVal != NAME_None)
		{
			const FName CurrentPath = FSoftObjectPath(this).GetAssetPathName();
			FName ExistsPath = NAME_None;
			if (UAssetMetaRegistrySource::IsAssetWithNameExists(CurrentPath, NewVal, ExistsPath))
			{
				const FText ErrorMessage = FText::Format(
					LOCTEXT("AssetNameAlreadyExists", "Ассет с таким именем уже существует: '{0}'."),
					FText::FromName(ExistsPath));
				const FText Caption = FText(LOCTEXT("AssetNameAlreadyExists", "Неверное имя ассета"));
				FMessageDialog::Open(EAppMsgType::Ok, ErrorMessage, &Caption);
				AssetName = NAME_None;
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE