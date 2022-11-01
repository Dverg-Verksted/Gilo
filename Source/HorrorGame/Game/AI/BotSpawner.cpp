// It is owned by the company Dverg Verksted.

#include "Game/AI/BotSpawner.h"
#include "DataRegistrySubsystem.h"
#include "Engine/AssetManager.h"
#include "DataAssets/BotDataAssetBase.h"
#include "Game/Common/AssetMetaRegistrySource.h"
#include "Game/Common/AssetSetupActor.h"

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
		if (const auto* BotAsset = Cast<UBotDataAssetBase>(Manager->GetPrimaryAssetObject(LoadedAssetID)))
		{
			InitFromAsset_Implementation(BotAsset);
		}
	}
}

void ABotSpawner::InitFromAsset_Implementation(const UPrimaryDataAsset* SourceAsset)
{
	auto* BotAsset = Cast<UBotDataAssetBase>(SourceAsset);
	if (!BotAsset || !BotAsset->BotClass) return;

	FTransform SpawnTransform;
	GetSpawnTransform(SpawnTransform);

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParameters.bDeferConstruction = true;

	auto* BotClass = BotAsset->BotClass.Get();
	if (auto* BotActor = GetWorld()->SpawnActor<AActor>(BotClass, SpawnTransform, SpawnParameters))
	{
		if (BotClass->ImplementsInterface(UAssetSetupActor::StaticClass()))
		{
			/* Инициализация бота */
			IAssetSetupActor::Execute_InitFromAsset(BotActor, BotAsset);
		}

		BotActor->FinishSpawning(SpawnTransform);
	}
}
