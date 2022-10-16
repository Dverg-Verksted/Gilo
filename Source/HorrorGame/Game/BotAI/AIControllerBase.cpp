// It is owned by the company Dverg Verksted.


#include "Game/BotAI/AIControllerBase.h"
#include "Game/Player/PlayerCharacterBase.h"
#include "Game/BotAI/Waypoint.h"
#include "BotBase.h"
#include "BotSettings.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AISenseConfig_Sight.h"

void AAIControllerBase::BeginPlay()
{
	Super::BeginPlay();
	ABotBase* BotBase = Cast<ABotBase>(GetPawn());
	if(BotBase)
	{
		Bot = BotBase;
	}
	bIsStay = false;
}

void AAIControllerBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if(bIsStay)
	{
		StopMovement();
		UE_LOG(LogTemp,Error,TEXT("STOP %b"),bIsStay);
	}
	else
	{
		UE_LOG(LogTemp,Error,TEXT("STOP %b"),bIsStay);
		if(bStimulusSensed)
		{
			MoveToActor(ActorBase,50.0f);
		}
		else
		{
			if(Bot->NextWaypoint != nullptr && bStimulusSensed == false)
			{
				MoveToActor(Bot->NextWaypoint, 0.0f);
			}
		}
	}
}

AAIControllerBase::AAIControllerBase(const FObjectInitializer& ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));

	Sight = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	if(const UBotSettings* BotSettings = GetDefault<UBotSettings>())
	{
		Sight->SightRadius = ISightRadius = BotSettings->ISightRadius;
		Sight->SetMaxAge(ISightAge = BotSettings->ISightAge);
		Sight->LoseSightRadius = ILoseSightRadius = BotSettings->ILoseSightRadius;
		Sight->PeripheralVisionAngleDegrees = IFieldOfView = BotSettings->IFieldOfView;
		bIsPlayerDetected = BotSettings->bIsPlayerDetected;
		DistanceToPlayer = BotSettings->DistanceToPlayer;
	}
	AIPerceptionComponent->ConfigureSense(*Sight);
	AIPerceptionComponent->SetDominantSense(Sight->GetSenseImplementation());
	AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AAIControllerBase::OnPerception);
}

void AAIControllerBase::OnPerception(AActor* Actor, FAIStimulus Stimulus)
{
	
	auto d = Cast<APlayerCharacterBase>(Actor);
	if (IsValid(Cast<APlayerCharacterBase>(Actor)))
	{
		ActorBase = Cast<APlayerCharacterBase>(Actor);
		bStimulusSensed = Stimulus.WasSuccessfullySensed();
		DistanceToPlayer = GetPawn()->GetDistanceTo(Actor);
	}
	
}

FRotator AAIControllerBase::GetControlRotation() const
{
	if (GetPawn() == nullptr)
	{
		return FRotator(0.0f, 0.0f, 0.0f);
	}
	return FRotator(0.0f, GetPawn()->GetActorRotation().Yaw, 0.0f);
}

void AAIControllerBase::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AAIControllerBase::OnPerception); 
}

void AAIControllerBase::CharacterIsSee(bool bStay)
{
	bIsStay = bStay;
}

