// It is owned by the company Dverg Verksted.

#include "Game/BotAI/BotSpawner.h"
#include "DataRegistrySubsystem.h"
#include "Engine/AssetManager.h"
#include "DataAssets/BotDataAssetBase.h"
#include "Game/BotAI/BotBase.h"
#include "Game/Common/AssetMetaRegistrySource.h"

// Sets default values
ABotSpawner::ABotSpawner()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ABotSpawner::BeginPlay()
{
	Super::BeginPlay();
	SpawnBot();
}

// Called every frame
void ABotSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABotSpawner::SpawnBot()
{
	const auto* RegistrySystem = UDataRegistrySubsystem::Get();
	if (!IsValid(RegistrySystem)) return;

	const UDataRegistry* Registry = RegistrySystem->GetRegistryForType(BotID.RegistryType);
	if (!Registry) return;

	UAssetManager* Manager = UAssetManager::GetIfValid();
	if (!Manager) return;

	if (auto* Asset = Registry->GetCachedItem<FAssetMetaRegistryRow>(BotID))
	{
		TArray<FName> Bundles;
		Bundles.Add(FName("classes"));
		const FStreamableDelegate Delegate = FStreamableDelegate::CreateUObject(this, &ABotSpawner::OnBotAssetLoaded, Asset->AssetID);
		Manager->LoadPrimaryAsset(Asset->AssetID, Bundles, Delegate);
	}
}

void ABotSpawner::OnBotAssetLoaded(FPrimaryAssetId LoadedAssetID)
{
	if (const auto* Manager = UAssetManager::GetIfValid())
	{
		if (auto* BotAsset = Cast<UBotDataAssetBase>(Manager->GetPrimaryAssetObject(LoadedAssetID)))
		{
			InitFromAsset_Implementation(BotAsset);
		}
	}
}

void ABotSpawner::InitFromAsset_Implementation(UPrimaryDataAsset* SourceAsset)
{
	const auto* BotAsset = Cast<UBotDataAssetBase>(SourceAsset);
	if (BotAsset || !BotAsset->BotClass) return;

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	auto* BotActor = GetWorld()->SpawnActor<ABotBase>(BotAsset->BotClass.Get(), SpawnParameters);
}
