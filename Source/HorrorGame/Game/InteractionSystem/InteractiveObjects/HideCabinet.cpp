// It is owned by the company Dverg Verksted.

#include "HideCabinet.h"
#include "DoorActorBase.h"
#include "Game/InteractionSystem/InteractionComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

#pragma optimize("", off)

AHideCabinet::AHideCabinet(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AHideCabinet::BeginPlay()
{
	Super::BeginPlay();
}

void AHideCabinet::OnUseObject_Implementation(APlayerController* PlayerController)
{
	if (bOccupied)
	{
		ReleaseCabinet(PlayerController);
	}
	else
	{
		OccupyCabinet(PlayerController);
	}
}

void AHideCabinet::OccupyCabinet(APlayerController* PlayerController)
{
	if (bOccupied) return;
	auto* Character = Cast<ACharacter>(PlayerController->GetPawn());
	if (!Character) return;
	if (auto* Movement = Cast<UCharacterMovementComponent>(Character->GetMovementComponent()))
	{
		Movement->DisableMovement();
	}
	auto* Interaction = UInteractionComponent::Get(PlayerController);
	if (Interaction && Interaction->GetLockedActor() != this)
	{
		Interaction->LockOnTarget(this);
	}
	OnPlayerEnter(PlayerController);
	bOccupied = true;
}

void AHideCabinet::ReleaseCabinet(APlayerController* PlayerController)
{
	if (!bOccupied) return;
	const auto* Character = Cast<ACharacter>(PlayerController->GetPawn());
	if (!Character) return;
	if (auto* Movement = Cast<UCharacterMovementComponent>(Character->GetMovementComponent()))
	{
		Movement->SetDefaultMovementMode();
	}
	if (auto* Interaction = UInteractionComponent::Get(PlayerController))
	{
		Interaction->ClearTargetLock();
	}
	OnPlayerExit(PlayerController);
	bOccupied = false;
}

void AHideCabinet::OpenAllDoors()
{
	TArray<ADoorActorBase*> Doors;
	GetDoors(Doors);
	for (auto* Door : Doors)
	{
		Door->OpenDoor();
	}
}

void AHideCabinet::CloseAllDoors()
{
	TArray<ADoorActorBase*> Doors;
	GetDoors(Doors);
	for (auto* Door : Doors)
	{
		Door->CloseDoor();
	}
}

void AHideCabinet::GetDoors(TArray<ADoorActorBase*>& Doors) const
{
	TArray<AActor*> DirectChildren;
	GetAllChildActors(DirectChildren, false);
	for (auto* Actor : DirectChildren)
	{
		if (auto* Door = Cast<ADoorActorBase>(Actor))
		{
			Doors.Add(Door);
		}
	}
}

#pragma optimize("", on)