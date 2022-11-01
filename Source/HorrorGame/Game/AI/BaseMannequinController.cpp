// It is owned by the company Dverg Verksted.

#include "BaseMannequinController.h"

ABaseMannequinController::ABaseMannequinController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	AAIController::SetGenericTeamId(FGenericTeamId(1));
}

void ABaseMannequinController::BeginPlay()
{
	Super::BeginPlay();
}

ETeamAttitude::Type ABaseMannequinController::GetTeamAttitudeTowards(const AActor& Other) const
{
	auto* OtherPawn = Cast<APawn>(&Other);
	if (!OtherPawn) return ETeamAttitude::Neutral;

	auto* OtherController = OtherPawn->GetController();
	if (!OtherController) return ETeamAttitude::Neutral;

	if (const auto* TeamAgent = Cast<IGenericTeamAgentInterface>(OtherController))
	{
		// Всех, кто не из нашей команды считаем врагами
		const FGenericTeamId CurrentTeamID = GetGenericTeamId();
		const FGenericTeamId OtherTeamID = TeamAgent->GetGenericTeamId();
		if (CurrentTeamID.GetId() != OtherTeamID.GetId()) return ETeamAttitude::Hostile;
	}

	return ETeamAttitude::Neutral;
}

void ABaseMannequinController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
