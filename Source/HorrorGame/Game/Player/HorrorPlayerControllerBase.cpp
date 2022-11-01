// It is owned by the company Dverg Verksted.

#include "HorrorPlayerControllerBase.h"

AHorrorPlayerControllerBase::AHorrorPlayerControllerBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	AHorrorPlayerControllerBase::SetGenericTeamId(FGenericTeamId(0));
}

void AHorrorPlayerControllerBase::BeginPlay()
{
	Super::BeginPlay();
}

void AHorrorPlayerControllerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AHorrorPlayerControllerBase::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	TeamID = NewTeamID;
}
