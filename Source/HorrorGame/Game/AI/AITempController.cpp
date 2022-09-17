// Fill out your copyright notice in the Description page of Project Settings.


#include "Bot/AITempController.h"
#include "Bot/AngelBot.h"
#include "Bot/PlayerBaseCharacter.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Bot/PlayerBaseCharacter.h"
#include "Bot/Waypoint.h"
#include "Runtime/AIModule/Classes/Perception/AISenseConfig_Sight.h"

void AAITempController::BeginPlay()
{
	Super::BeginPlay();
	AAngelBot* Chr = Cast<AAngelBot>(GetPawn());
	if(Chr)
	{
		Agent = Chr;
	}
}

void AAITempController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(DistanceToPlayer > ISightRadius)
	{
		bIsPlayerDetected = false;
		StopMovement();
	}
	if(Agent->NextWaypoint != nullptr && bIsPlayerDetected == false)
	{
		MoveToActor(Agent->NextWaypoint,15.0f);
	}
	else if (bIsPlayerDetected == true)
	{
		APlayerBaseCharacter* BasePlayer = Cast<APlayerBaseCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(),0));
		MoveToActor(BasePlayer, 15.0f);
		
	}
}

void AAITempController::OnPawnDetected(const TArray<AActor*>& DetectedPawns)
{
	for (size_t i = 0; i < DetectedPawns.Num(); i++)
	{
		DistanceToPlayer= GetPawn()->GetDistanceTo(DetectedPawns[i]);
	}

	bIsPlayerDetected = true;
}

AAITempController::AAITempController(const FObjectInitializer& ObjectInitializer)
{

	PrimaryActorTick.bCanEverTick = true;
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));
	
	Sight = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));

	Sight->SightRadius = ISightRadius;
	Sight->LoseSightRadius = ILoseSightRadius;
	Sight->PeripheralVisionAngleDegrees = IFieldOfView;
	Sight->SetMaxAge(ISightAge);
	Sight->DetectionByAffiliation.bDetectNeutrals = true;

	AIPerceptionComponent->ConfigureSense(*Sight);
	AIPerceptionComponent->SetDominantSense(Sight->GetSenseImplementation());
	AIPerceptionComponent->OnPerceptionUpdated.AddDynamic(this, &AAITempController::OnPawnDetected); 
}

void AAITempController::OnPerception(AActor* Actor, FAIStimulus Stimulus)
{
	//APlayerBaseCharacter* Chr = Cast<APlayerBaseCharacter>(Actor);
	//if(Chr == nullptr) return;
	//SetFocus(Stimulus.WasSuccessfullySensed()?Chr:nullptr);
	//MoveToActor(Chr,10.0f);
}

FRotator AAITempController::GetControlRotation() const
{
	if (GetPawn() == nullptr)
	{
		return FRotator(0.0f, 0.0f, 0.0f);
	}
	return FRotator(0.0f, GetPawn()->GetActorRotation().Yaw, 0.0f);
}


void AAITempController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this,&AAITempController::OnPerception);
}
