// It is owned by the company Dverg Verksted.


#include "Game/InteractionSystem/InteractionComponent.h"

#include "InteractiveObject.h"
#include "Kismet/KismetSystemLibrary.h"

UInteractionComponent::UInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.15f;
	TraceDistance = 100.0f;
	TraceRadius = 10.0f;
	bTracingEnabled = false;

	PlayerController = nullptr;
	PlayerPawn = nullptr;
	LastInteractiveObject = nullptr;
}


void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UInteractionComponent::Trace(const FVector& Start, const FVector& Stop)
{
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(PlayerController);
	IgnoreActors.Add(PlayerPawn);

	TArray<FHitResult> Hits;

	UKismetSystemLibrary::SphereTraceMultiForObjects(this, Start, Stop, TraceRadius, TraceObjectTypes, false, IgnoreActors,
		EDrawDebugTrace::None, Hits, true);

	float MinDistance = TraceDistance;
	bool HasResult = false;
	FHitResult ResultHit;
	AActor* ResultActor = nullptr;

	for (const auto& Hit : Hits)
	{
		AActor* HitActor = Hit.GetActor();
		if (!HitActor)
		{
			// Пропускаем херню
			continue;
		}

		if (!HitActor->GetClass()->ImplementsInterface(UInteractiveObject::StaticClass()))
		{
			// Пропускаем не интерактивные объекты
			continue;
		}

		if (HasResult && Hit.Distance >= MinDistance)
		{
			// Фильтруем объекты по расстоянию от камеры
			continue;
		}

		ResultHit = Hit;
		ResultActor = HitActor;
		MinDistance = Hit.Distance;
		HasResult = true;
	}

	if (!HasResult)
	{
		ClearLastInteractionObject();
	}
	else
	{
		if (ResultActor != LastInteractiveObject)
		{
			SelectNewInteractionObject(ResultHit);
		}
	}
}

void UInteractionComponent::SelectNewInteractionObject(const FHitResult& Hit)
{
	ClearLastInteractionObject();

	if (AActor* NewInteractiveActor = Hit.GetActor())
	{
		LastInteractiveObject = NewInteractiveActor;
		IInteractiveObject::Execute_OnHoverBegin(NewInteractiveActor, PlayerController, Hit);
		OnInteractionObjectChanged.Broadcast(LastInteractiveObject, Hit);
	}
}

void UInteractionComponent::ClearLastInteractionObject()
{
	if (LastInteractiveObject)
	{
		IInteractiveObject::Execute_OnHoverEnd(LastInteractiveObject, PlayerController);
		LastInteractiveObject = nullptr;
	}
}

void UInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (bTracingEnabled)
	{
		if (PlayerController)
		{
			const FVector Start = PlayerController->PlayerCameraManager->GetCameraLocation();
			const FVector Forward = PlayerController->PlayerCameraManager->GetCameraRotation().Quaternion().
			                                          GetForwardVector();
			const FVector Stop = Start + Forward * TraceDistance;
			Trace(Start, Stop);
		}
	}
}

void UInteractionComponent::StartTrace()
{
	if (APlayerController* Controller = Cast<APlayerController>(GetOwner()))
	{
		PlayerController = Controller;
		PlayerPawn = PlayerController->GetPawnOrSpectator();
		bTracingEnabled = true;
	}
}

void UInteractionComponent::StopTrace()
{
	bTracingEnabled = false;
}
