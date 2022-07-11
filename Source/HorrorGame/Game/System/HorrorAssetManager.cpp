// It is owned by the company Dverg Verksted.

#include "Game/System/HorrorAssetManager.h"

UHorrorAssetManager::UHorrorAssetManager() {}

UHorrorAssetManager* UHorrorAssetManager::Get()
{
	ensure(GEngine);
	if (auto* Singleton = Cast<UHorrorAssetManager>(GEngine->AssetManager))
	{
		return Singleton;
	}

	UE_LOG(LogTemp, Error, TEXT("В файле конфигурации задан неверный класс Ассет Менеджера.  Установите класс HorrorAssetManager"));
	return nullptr;
}

UObject* UHorrorAssetManager::LoadAssetSync(const FSoftObjectPath& AssetPath) const
{
	if (AssetPath.IsValid())
	{
		if (ShouldLogAssetLoads())
		{
			TUniquePtr<FScopeLogTime> LogTimePtr =
				MakeUnique<FScopeLogTime>(*FString::Printf(TEXT("Synchronously loaded asset [%s]"), *AssetPath.ToString()), nullptr, FScopeLogTime::ScopeLog_Seconds);
		}
		if (UAssetManager::IsValid())
		{
			return UAssetManager::GetStreamableManager().LoadSynchronous(AssetPath, false);
		}
		// Use LoadObject if asset manager isn't ready yet.
		return AssetPath.TryLoad();
	}
	return nullptr;
}

bool UHorrorAssetManager::ShouldLogAssetLoads()
{
	static bool bLogAssetLoads = FParse::Param(FCommandLine::Get(), TEXT("LogAssetLoads"));
	return bLogAssetLoads;
}
