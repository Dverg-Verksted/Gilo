// It is owned by the company Dverg Verksted.

#include "Game/BotAI/BotSpawner.h"
#include "DataRegistrySubsystem.h"
#include "Engine/AssetManager.h"
#include "DataAssets/BotDataAssetBase.h"
#include "Game/BotAI/BotBase.h"
#include "Game/Common/AssetMetaRegistrySource.h"

ABotSpawner::ABotSpawner(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
	SetCanBeDamaged(false);
	SetHidden(true);

	SceneRoot = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("SceneRoot"));
	RootComponent = SceneRoot;
}

void ABotSpawner::BeginPlay()
{
	Super::BeginPlay();
	SpawnBot();
}

void ABotSpawner::GetSpawnTransform_Implementation(FTransform& SpawnTransform)
{
	SpawnTransform = SceneRoot->GetComponentTransform();
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

void ABotSpawner::InitFromAsset_Implementation(const UPrimaryDataAsset* SourceAsset)
{
	const auto* BotAsset = Cast<UBotDataAssetBase>(SourceAsset);
	if (!BotAsset || !BotAsset->BotClass) return;

	FTransform SpawnTransform;
	GetSpawnTransform(SpawnTransform);

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParameters.bDeferConstruction = true;
	auto* BotActor = GetWorld()->SpawnActor<ABotBase>(BotAsset->BotClass.Get(), SpawnTransform, SpawnParameters);
	if (BotActor)
	{
		BotActor->InitFromAsset(BotAsset);
		BotActor->FinishSpawning(SpawnTransform);
	}
}
