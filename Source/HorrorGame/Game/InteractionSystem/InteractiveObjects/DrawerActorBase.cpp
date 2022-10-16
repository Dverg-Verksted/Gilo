// It is owned by the company Dverg Verksted.

#include "Game/InteractionSystem/InteractiveObjects/DrawerActorBase.h"
#include "DataRegistrySubsystem.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/AssetManager.h"
#include "Game/Common/AssetMetaRegistrySource.h"
#include "Game/InteractionSystem/InteractionComponent.h"
#include "Game/InteractionSystem/InteractionSettings.h"
#include "Game/InteractionSystem/DataAssets/DrawerDataAsset.h"
#include "Kismet/KismetMathLibrary.h"

ADrawerActorBase::ADrawerActorBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	SceneRoot = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("SceneRoot"));
	RootComponent = SceneRoot;

	DrawerRootComponent = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("DrawerRootComponent"));
	DrawerRootComponent->SetupAttachment(RootComponent);

	DrawerMeshComponent = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("DrawerMeshComponent"));
	DrawerMeshComponent->SetupAttachment(DrawerRootComponent);

	DragPlayerController = nullptr;
}

void ADrawerActorBase::BeginPlay()
{
	Super::BeginPlay();
}

void ADrawerActorBase::DrawerDragActionHandler(const FInputActionValue& ActionValue)
{
	if (!DragPlayerController) return;

	FVector NewLocation = DrawerRootComponent->GetRelativeLocation();
	float Extend = NewLocation.X;
	Extend += ActionValue.Get<float>() * DragMagnitude;
	Extend = FMath::Clamp(Extend, 0.0f, MaxExtendDistance);
	NewLocation.X = Extend;
	DrawerRootComponent->SetRelativeLocation(NewLocation, true);
	GEngine->AddOnScreenDebugMessage(156, 0.35f, FColor::Emerald, NewLocation.ToCompactString());
}

void ADrawerActorBase::GrabObjectTriggeredHandler(const FInputActionValue& ActionValue)
{
	if (auto* Comp = UInteractionComponent::Get(DragPlayerController))
	{
		if (Comp->GetLockedActor() != this)
		{
			Comp->LockOnTarget(this);
		}
	}
}

void ADrawerActorBase::GrabObjectCompletedHandler(const FInputActionValue& ActionValue)
{
	if (auto* Comp = UInteractionComponent::Get(DragPlayerController))
	{
		Comp->ClearTargetLock();
	}
}

void ADrawerActorBase::OnDrawerAssetLoaded(FPrimaryAssetId LoadedAssetID)
{
	const auto* Manager = UAssetManager::GetIfValid();
	if (!Manager) return;

	if (auto* DoorData = Cast<UDrawerDataAsset>(Manager->GetPrimaryAssetObject(LoadedAssetID)))
	{
		InitFromAsset_Implementation(DoorData);
	}
}

void ADrawerActorBase::ReloadDrawerAsset()
{
	const auto* RegistrySystem = UDataRegistrySubsystem::Get();
	if (!IsValid(RegistrySystem)) return;

	const UDataRegistry* Registry = RegistrySystem->GetRegistryForType(DrawerID.RegistryType);
	if (!Registry) return;

	auto* Manager = UAssetManager::GetIfValid();
	if (!Manager) return;

	if (auto* Asset = Registry->GetCachedItem<FAssetMetaRegistryRow>(DrawerID))
	{
		TArray<FName> Bundles;
		Bundles.Add(FName("meshes"));
		const FStreamableDelegate Delegate = FStreamableDelegate::CreateUObject(this, &ADrawerActorBase::OnDrawerAssetLoaded, Asset->AssetID);
		Manager->LoadPrimaryAsset(Asset->AssetID, Bundles, Delegate);
	}
}

void ADrawerActorBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	ReloadDrawerAsset();
}

void ADrawerActorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bMoving)
	{
		const FVector CurrentLocation = DrawerRootComponent->GetRelativeLocation();
		const FVector TargetLocation = FVector(MovingTargetDepth, CurrentLocation.Y, CurrentLocation.Z);
		const FVector NewLocation = UKismetMathLibrary::VInterpTo(CurrentLocation, TargetLocation, DeltaTime, AutoOpenSpeed);
		if (FMath::IsNearlyEqual(FMath::Abs(CurrentLocation.X), FMath::Abs(MovingTargetDepth), 0.1f))
		{
			DrawerRootComponent->SetRelativeLocation(TargetLocation);
			StopMoving();
			OnQuickOpenCloseComplete.Broadcast(MovingTargetDepth);
		}
		else
		{
			DrawerRootComponent->SetRelativeLocation(NewLocation);
		}
	}
}

void ADrawerActorBase::OnUseObject_Implementation(APlayerController* PlayerController)
{
	if (bDragged) return;
	if (bMoving) StopMoving();
	const float TargetDepth = CalculateDepth();
	StartMoving(TargetDepth);
}

void ADrawerActorBase::InitFromAsset_Implementation(UPrimaryDataAsset* SourceAsset)
{
	if (const auto* DrawerAsset = Cast<UDrawerDataAsset>(SourceAsset))
	{
		DrawerMeshComponent->SetStaticMesh(DrawerAsset->DrawerMesh.Get());
		if (const auto* Settings = UInteractionSettings::Get())
		{
			DrawerMeshComponent->SetCollisionProfileName(Settings->CollisionProfile);
		}
		MaxExtendDistance = DrawerAsset->MaxExtendDistance;
		DragMagnitude = DrawerAsset->DragMagnitude;
		AutoOpenSpeed = DrawerAsset->QuickOpenSpeed;
	}
}

void ADrawerActorBase::OnHoverBegin_Implementation(APlayerController* PlayerController, const FHitResult& Hit)
{
	IInteractiveObject::OnHoverBegin_Implementation(PlayerController, Hit);
	EnableInput(PlayerController);
	DragPlayerController = PlayerController;

	if (!bInputBinded && DrawerDragAction && GrabObjectAction)
	{
		// Привязка к вводу
		if (auto* EIC = Cast<UEnhancedInputComponent>(InputComponent))
		{
			EIC->BindAction(DrawerDragAction, ETriggerEvent::Triggered, this, &ADrawerActorBase::DrawerDragActionHandler);
			EIC->BindAction(GrabObjectAction, ETriggerEvent::Triggered, this, &ADrawerActorBase::GrabObjectTriggeredHandler);
			EIC->BindAction(GrabObjectAction, ETriggerEvent::Completed, this, &ADrawerActorBase::GrabObjectCompletedHandler);
			EIC->BindAction(GrabObjectAction, ETriggerEvent::Canceled, this, &ADrawerActorBase::GrabObjectCompletedHandler);
			bInputBinded = true;
		}
	}

	if (auto* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
	{
		if (DrawerInputContext && !Subsystem->HasMappingContext(DrawerInputContext))
		{
			// Добавляем контекст ввода ящика
			FModifyContextOptions Options;
			Options.bIgnoreAllPressedKeysUntilRelease = false;
			Subsystem->AddMappingContext(DrawerInputContext, DrawerInputContextPriority, Options);
		}
	}
}

void ADrawerActorBase::OnHoverEnd_Implementation(APlayerController* PlayerController)
{
	IInteractiveObject::OnHoverEnd_Implementation(PlayerController);
	DisableInput(PlayerController);
	DragPlayerController = nullptr;

	if (auto* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
	{
		if (DrawerInputContext && Subsystem->HasMappingContext(DrawerInputContext))
		{
			// Удаляем контекст ввода ящика
			FModifyContextOptions Options;
			Options.bIgnoreAllPressedKeysUntilRelease = false;
			Subsystem->RemoveMappingContext(DrawerInputContext, Options);
		}
	}
}

#if WITH_EDITOR
void ADrawerActorBase::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (PropertyChangedEvent.MemberProperty && PropertyChangedEvent.MemberProperty->GetFName() == GET_MEMBER_NAME_CHECKED(ADrawerActorBase, DrawerID))
	{
		ReloadDrawerAsset();
	}
}
#endif

bool ADrawerActorBase::IsMoving() const
{
	if (bDragged || bMoving) return true;
	return false;
}

void ADrawerActorBase::StartMoving(float TargetDepth)
{
	if (IsMoving()) return;
	bMoving = true;

	const FVector CurrentLocation = DrawerRootComponent->GetRelativeLocation();
	MovingStartDepth = CurrentLocation.X;
	MovingTargetDepth = FMath::Clamp(TargetDepth, 0.0f, MaxExtendDistance);
	SetActorTickEnabled(true);
}

void ADrawerActorBase::StopMoving()
{
	SetActorTickEnabled(false);
	bMoving = false;
}

float ADrawerActorBase::CalculateDepth() const
{
	const FVector CurrentLocation = DrawerRootComponent->GetRelativeLocation();
	const FVector ClosedLocation = FVector(0.0f, CurrentLocation.Y, CurrentLocation.Z);
	const FVector OpenedLocation = FVector(MaxExtendDistance, CurrentLocation.Y, CurrentLocation.Z);
	const float DistMin = FVector::Distance(CurrentLocation, ClosedLocation);
	const float DistMax = FVector::Distance(CurrentLocation, OpenedLocation);
	return DistMin >= DistMax ? 0.0f : MaxExtendDistance;
}
