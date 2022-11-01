// It is owned by the company Dverg Verksted.

#include "BaseMannequin.h"
#include "BaseMannequinController.h"
#include "DataAssets/BotDataAssetBase.h"

ABaseMannequin::ABaseMannequin(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	PerceptionComponent = ObjectInitializer.CreateDefaultSubobject<UAIPerceptionComponent>(this, TEXT("PerceptionComponent"));
	AIControllerClass = ABaseMannequinController::StaticClass();
}

void ABaseMannequin::BeginPlay()
{
	Super::BeginPlay();
}

void ABaseMannequin::InitFromAsset_Implementation(const UPrimaryDataAsset* SourceAsset)
{
	auto* BotAsset = Cast<UBotDataAssetBase>(SourceAsset);
	if (!BotAsset) return;

	WalkSpeed = BotAsset->BotWalkSpeed;
	RunSpeed = BotAsset->BotRunSpeed;
}

void ABaseMannequin::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
