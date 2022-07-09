// It is owned by the company Dverg Verksted.

#include "Game/InteractionSystem/InteractionComponent.h"
#include "EnhancedInputComponent.h"
#include "InteractionSettings.h"
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
	LockTargetActor = nullptr;
}

void UInteractionComponent::OnPlayerReady()
{
	APawn* Pawn = Cast<APawn>(GetOwner());
	ensure(Pawn);
	if (!Pawn) return;

	APlayerController* Controller = Cast<APlayerController>(Pawn->GetController());
	ensure(Controller);
	if (!Controller) return;

	PlayerController = Controller;
	PlayerPawn = Pawn;
	PhysicsHandleComponent = Cast<UPhysicsHandleComponent>(Pawn->GetComponentByClass(UPhysicsHandleComponent::StaticClass()));

	ensure(PhysicsHandleComponent);

	UEnhancedInputComponent* InputComponent = Cast<UEnhancedInputComponent>(Controller->InputComponent);
	if (!InputComponent) return;

	ensure(UseAction);
	ensure(GrabAction);
	ensure(PushGrabbedAction);
	ensure(GrabbedRotationAction);

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
	if (PushGrabbedAction)
	{
		InputComponent->BindAction(PushGrabbedAction, ETriggerEvent::Triggered, this, &UInteractionComponent::PushGrabbedActionHandler);
	}
	if (GrabbedRotationAction)
	{
		InputComponent->BindAction(GrabbedRotationAction, ETriggerEvent::Triggered, this, &UInteractionComponent::GrabbedRotationActionHandler);
	}
}

void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	if (auto* Settings = UInteractionSettings::Get())
	{
		TraceDistance = Settings->TraceDistance;
		TraceRadius = Settings->TraceRadius;
	}

	if (TraceDistance > 0.0f && TraceRadius > 0.0f)
	{
		GetWorld()->GetTimerManager().SetTimer(TraceTimerHandle, this, &UInteractionComponent::OnTraceTimerTick, TraceTimerInterval, true);
	}
}

void UInteractionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	GetWorld()->GetTimerManager().ClearTimer(TraceTimerHandle);
}

void UInteractionComponent::OnTraceTimerTick()
{
	if (bTracingEnabled && !bGrabbingObject && !bIsLocked)
	{
		const FVector Start = PlayerController->PlayerCameraManager->GetCameraLocation();
		const FVector Forward = PlayerController->PlayerCameraManager->GetCameraRotation().Quaternion().GetForwardVector();
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

	UKismetSystemLibrary::SphereTraceMultiForObjects(this, Start, Stop, TraceRadius, TraceObjectTypes, false, IgnoreActors, EDrawDebugTrace::None, Hits, true);

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

		if (HitActor->GetVelocity().Length() != 0.0f)
		{
			// Игнорируем движущиеся объекты
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
			SelectNewInteractionObject(ResultHit, ResultActor);
		}
	}
}

void UInteractionComponent::SelectNewInteractionObject(const FHitResult& Hit, AActor* Actor)
{
	ClearLastInteractionObject();

	if (!Actor) return;

	LastInteractiveObject = Actor;
	IInteractiveObject::Execute_OnHoverBegin(Actor, PlayerController, Hit);
	OnHoverBegin.Broadcast(Actor, Hit);

	// Уведомляем дочерние компоненты актора, реализующий интерфейс InteractiveObject
	TArray<UActorComponent*> InteractionListeners;
	Actor->GetComponents(InteractionListeners, false);
	for (auto* Comp : InteractionListeners)
	{
		if (Comp->GetClass()->ImplementsInterface(UInteractiveObject::StaticClass()))
		{
			IInteractiveObject::Execute_OnHoverBegin(Comp, PlayerController, Hit);
		}
	}

	OnInteractionObjectChanged.Broadcast(LastInteractiveObject, Hit);
}

void UInteractionComponent::ClearLastInteractionObject()
{
	if (LastInteractiveObject)
	{
		IInteractiveObject::Execute_OnHoverEnd(LastInteractiveObject, PlayerController);
		OnHoverEnd.Broadcast(LastInteractiveObject);

		// Уведомляем дочерние компоненты актора, реализующий интерфейс InteractiveObject
		TArray<UActorComponent*> InteractionListeners;
		LastInteractiveObject->GetComponents(InteractionListeners, false);
		for (auto* Comp : InteractionListeners)
		{
			if (Comp->GetClass()->ImplementsInterface(UInteractiveObject::StaticClass()))
			{
				IInteractiveObject::Execute_OnHoverEnd(Comp, PlayerController);
			}
		}
	}
	LastInteractiveObject = nullptr;
}

void UInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!PlayerController) return;

	if (bGrabbingObject)
	{
		const FVector Start = PlayerController->PlayerCameraManager->GetCameraLocation();
		const FVector Forward = PlayerController->PlayerCameraManager->GetCameraRotation().Quaternion().GetForwardVector();
		const FVector TargetLocation = Start + Forward * GrabDistance;
		if (auto* Comp = PhysicsHandleComponent->GetGrabbedComponent())
		{
			if (!Comp->IsAnyRigidBodyAwake())
			{
				//Если компонент уснул - пинаем этого урода
				Comp->WakeAllRigidBodies();
			}
			PhysicsHandleComponent->SetTargetLocationAndRotation(TargetLocation, GrabbedDesiredRotation);
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

void UInteractionComponent::LockOnTarget(AActor* Actor)
{
	if (IsValid(LockTargetActor)) ClearTargetLock();

	if (IsValid(Actor))
	{
		bIsLocked = true;
		if (LastInteractiveObject != Actor)
		{
			SelectNewInteractionObject(FHitResult(), Actor);
		}
		LockTargetActor = Actor;
		OnInteractionTargetLock.Broadcast(LockTargetActor);
	}
}

void UInteractionComponent::ClearTargetLock()
{
	if (bGrabbingObject) return;

	if (LockTargetActor) OnInteractionTargetUnlock.Broadcast(LockTargetActor);

	LockTargetActor = nullptr;
	bIsLocked = false;
	ClearLastInteractionObject();
}

UInteractionComponent* UInteractionComponent::Get(const APlayerController* PlayerController)
{
	if (PlayerController)
	{
		if (const auto* Pawn = PlayerController->GetPawn())
		{
			return Cast<UInteractionComponent>(Pawn->GetComponentByClass(StaticClass()));
		}
	}
	return nullptr;
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
	if (bGrabbingObject || bGrabTriggered) return;

	if (LastInteractiveObject)
	{
		bool bCanBeGrabbed = false;
		UPrimitiveComponent* ComponentToGrab = nullptr;
		IInteractiveObject::Execute_CanBeGrabbed(LastInteractiveObject, bCanBeGrabbed, ComponentToGrab);
		if (bCanBeGrabbed)
		{
			ComponentToGrab->WakeAllRigidBodies();
			GrabbedDesiredRotation = ComponentToGrab->GetComponentRotation();
			PhysicsHandleComponent->GrabComponentAtLocationWithRotation(ComponentToGrab, NAME_None, ComponentToGrab->GetComponentLocation(), GrabbedDesiredRotation);
			IInteractiveObject::Execute_OnObjectGrabbed(LastInteractiveObject);
			bGrabbingObject = true;
		}
	}
	bGrabTriggered = true;
}

void UInteractionComponent::ReleaseGrabbedObject()
{
	if (bGrabbingObject)
	{
		PhysicsHandleComponent->ReleaseComponent();
		if (LastInteractiveObject)
		{
			IInteractiveObject::Execute_OnObjectReleased(LastInteractiveObject);
		}
		bGrabbingObject = false;
		ClearLastInteractionObject();
	}
}

void UInteractionComponent::GrabActionStopHandler(const FInputActionValue& ActionValue)
{
	ReleaseGrabbedObject();
	bGrabTriggered = false;
}

void UInteractionComponent::PushGrabbedActionHandler(const FInputActionValue& ActionValue)
{
	if (bGrabbingObject)
	{
		if (auto* GrabbedComp = PhysicsHandleComponent->GetGrabbedComponent())
		{
			// Отпускаем схваченный предмет
			ReleaseGrabbedObject();

			// Задаем импульс в сторону камеры
			const FVector Impulse = PlayerController->PlayerCameraManager->GetCameraRotation().Quaternion().GetForwardVector() * PushGrabbedForce;
			GrabbedComp->AddImpulse(Impulse, NAME_None);
		}
	}
}

void UInteractionComponent::GrabbedRotationActionHandler(const FInputActionValue& ActionValue)
{
	const FVector InputAxis = ActionValue.Get<FVector>();
	GrabbedDesiredRotation = GrabbedDesiredRotation.Add(InputAxis.Y, InputAxis.X, 0.0f);
}
