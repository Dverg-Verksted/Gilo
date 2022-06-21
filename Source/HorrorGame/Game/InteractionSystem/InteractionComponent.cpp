// It is owned by the company Dverg Verksted.


#include "Game/InteractionSystem/InteractionComponent.h"

#include "EnhancedInputComponent.h"
#include "InteractiveObject.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"

UInteractionComponent::UInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	TraceDistance = 100.0f;
	TraceRadius = 10.0f;
	bTracingEnabled = false;

	PlayerController = nullptr;
	PlayerPawn = nullptr;
	LastInteractiveObject = nullptr;
	PhysicsHandleComponent = nullptr;
}


void UInteractionComponent::OnPlayerReady()
{
	if (APawn* Pawn = Cast<APawn>(GetOwner()))
	{
		if (auto* Controller = Cast<APlayerController>(Pawn->GetController()))
		{
			PlayerController = Controller;
			PlayerPawn = Pawn;
			PhysicsHandleComponent = Cast<UPhysicsHandleComponent>(Pawn->GetComponentByClass(UPhysicsHandleComponent::StaticClass()));

			if (auto* InputComponent = Cast<UEnhancedInputComponent>(Controller->InputComponent))
			{
				ensure(UseAction);
				ensure(GrabAction);

				if (UseAction)
				{
					InputComponent->BindAction(UseAction, ETriggerEvent::Triggered, this, &UInteractionComponent::UseActionHandler);
				}
				if (GrabAction)
				{
					InputComponent->BindAction(GrabAction, ETriggerEvent::Triggered, this, &UInteractionComponent::GrabActionHandler);
					InputComponent->BindAction(GrabAction, ETriggerEvent::Completed, this, &UInteractionComponent::GrabActionStopHandler);
					InputComponent->BindAction(GrabAction, ETriggerEvent::Canceled, this, &UInteractionComponent::GrabActionStopHandler);
				}
			}
		}
	}

	ensure(PlayerController);
	ensure(PlayerPawn);
	ensure(PhysicsHandleComponent);
}

void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetTimerManager().SetTimer(TraceTimerHandle, this, &UInteractionComponent::OnTraceTimerTick, TraceTimerInterval, true);
}

void UInteractionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	GetWorld()->GetTimerManager().ClearTimer(TraceTimerHandle);
}

void UInteractionComponent::OnTraceTimerTick()
{
	if (bTracingEnabled && !bGrabbingObject)
	{
		const FVector Start = PlayerController->PlayerCameraManager->GetCameraLocation();
		const FVector Forward = PlayerController->PlayerCameraManager->GetCameraRotation().Quaternion().
		                                          GetForwardVector();
		const FVector Stop = Start + Forward * TraceDistance;
		Trace(Start, Stop);
	}
}

void UInteractionComponent::Trace(const FVector& Start, const FVector& Stop)
{
	if (!IsValid(LastInteractiveObject))
	{
		// На случай если последний интерактивный объект был удален
		ClearLastInteractionObject();
	}

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
	}
	LastInteractiveObject = nullptr;
}


void UInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!PlayerController)
		return;

	if (bGrabbingObject)
	{
		const FVector Start = PlayerController->PlayerCameraManager->GetCameraLocation();
		const FVector Forward = PlayerController->PlayerCameraManager->GetCameraRotation().Quaternion().
		                                          GetForwardVector();
		const FVector TargetLocation = Start + Forward * GrabDistance;
		if (auto* Comp = PhysicsHandleComponent->GetGrabbedComponent())
		{
			if (!Comp->IsAnyRigidBodyAwake())
			{
				// Если компонент уснул - пинаем этого урода
				Comp->WakeAllRigidBodies();
			}
			PhysicsHandleComponent->SetTargetLocation(TargetLocation);
		}
	}
}

void UInteractionComponent::StartTrace()
{
	if (PlayerController)
	{
		bTracingEnabled = true;
	}
}

void UInteractionComponent::StopTrace()
{
	bTracingEnabled = false;
}

void UInteractionComponent::UseActionHandler(const FInputActionValue& ActionValue)
{
	if (LastInteractiveObject)
	{
		IInteractiveObject::Execute_OnUseObject(LastInteractiveObject, PlayerController);
	}
}

void UInteractionComponent::GrabActionHandler(const FInputActionValue& ActionValue)
{
	if (bGrabbingObject)
		return;

	if (LastInteractiveObject)
	{
		bool bCanBeGrabbed = false;
		UPrimitiveComponent* ComponentToGrab = nullptr;
		IInteractiveObject::Execute_CanBeGrabbed(LastInteractiveObject, bCanBeGrabbed, ComponentToGrab);
		if (bCanBeGrabbed)
		{
			ComponentToGrab->WakeAllRigidBodies();
			PhysicsHandleComponent->GrabComponentAtLocation(ComponentToGrab, NAME_None, ComponentToGrab->GetComponentLocation());
			IInteractiveObject::Execute_OnObjectGrabbed(LastInteractiveObject);
			bGrabbingObject = true;
		}
	}
}

void UInteractionComponent::GrabActionStopHandler(const FInputActionValue& ActionValue)
{
	if (bGrabbingObject)
	{
		PhysicsHandleComponent->ReleaseComponent();
		if (LastInteractiveObject)
		{
			IInteractiveObject::Execute_OnObjectReleased(LastInteractiveObject);
		}
		bGrabbingObject = false;
	}
}
