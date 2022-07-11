// It is owned by the company Dverg Verksted.

#include "Game/InteractionSystem/InteractiveObjects/DoorActorBase.h"
#include "DataRegistrySubsystem.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/AssetManager.h"
#include "Game/Common/AssetMetaRegistrySource.h"
#include "Game/InteractionSystem/InteractionComponent.h"
#include "Game/InteractionSystem/InteractionSettings.h"
#include "Game/InteractionSystem/DataAssets/DoorDataAsset.h"

ADoorActorBase::ADoorActorBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("SceneRoot"));
	RootComponent = SceneRoot;

	DoorRootComponent = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("DoorRootComponent"));
	DoorRootComponent->SetupAttachment(RootComponent);

	DoorMeshComponent = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("DoorMeshComponent"));
	DoorMeshComponent->SetupAttachment(DoorRootComponent);

	DragPlayerController = nullptr;
}

void ADoorActorBase::BeginPlay()
{
	Super::BeginPlay();
}

void ADoorActorBase::ReloadDoorAsset()
{
	const auto* RegistrySystem = UDataRegistrySubsystem::Get();
	if (!IsValid(RegistrySystem)) return;

	const UDataRegistry* Registry = RegistrySystem->GetRegistryForType(DoorID.RegistryType);
	if (!Registry) return;

	UAssetManager* Manager = UAssetManager::GetIfValid();
	if (!Manager) return;

	if (auto* Asset = Registry->GetCachedItem<FAssetMetaRegistryRow>(DoorID))
	{
		TArray<FName> Bundles;
		Bundles.Add(FName("meshes"));
		const FStreamableDelegate Delegate = FStreamableDelegate::CreateUObject(this, &ADoorActorBase::OnDoorAssetLoaded, Asset->AssetID);
		Manager->LoadPrimaryAsset(Asset->AssetID, Bundles, Delegate);
	}
}

void ADoorActorBase::GrabObjectTriggeredHandler(const FInputActionValue& ActionValue)
{
	if (auto* Comp = UInteractionComponent::Get(DragPlayerController))
	{
		if (Comp->GetLockedActor() != this)
		{
			Comp->LockOnTarget(this);
		}
	}
}

void ADoorActorBase::GrabObjectCompletedHandler(const FInputActionValue& ActionValue)
{
	if (auto* Comp = UInteractionComponent::Get(DragPlayerController))
	{
		Comp->ClearTargetLock();
	}
}

void ADoorActorBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	ReloadDoorAsset();
}

void ADoorActorBase::InitFromAsset_Implementation(UPrimaryDataAsset* SourceAsset)
{
	if (const auto* DoorAsset = Cast<UDoorDataAsset>(SourceAsset))
	{
		DoorMeshComponent->SetStaticMesh(DoorAsset->DoorMesh.Get());
		if (const auto* Settings = UInteractionSettings::Get())
		{
			DoorMeshComponent->SetCollisionProfileName(Settings->CollisionProfile);
		}
		MinDoorAngle = DoorAsset->MinDoorAngle;
		MaxDoorAngle = DoorAsset->MaxDoorAngle;
		DragMagnitude = DoorAsset->DragMagnitude;
	}
}

void ADoorActorBase::OnHoverBegin_Implementation(APlayerController* PlayerController, const FHitResult& Hit)
{
	IInteractiveObject::OnHoverBegin_Implementation(PlayerController, Hit);
	EnableInput(PlayerController);
	DragPlayerController = PlayerController;

	if (!bInputBinded && DoorDragAction && GrabObjectAction)
	{
		// Привязка к вводу
		if (auto* EIC = Cast<UEnhancedInputComponent>(InputComponent))
		{
			EIC->BindAction(DoorDragAction, ETriggerEvent::Triggered, this, &ADoorActorBase::DoorDragActionHandler);
			EIC->BindAction(GrabObjectAction, ETriggerEvent::Triggered, this, &ADoorActorBase::GrabObjectTriggeredHandler);
			EIC->BindAction(GrabObjectAction, ETriggerEvent::Completed, this, &ADoorActorBase::GrabObjectCompletedHandler);
			EIC->BindAction(GrabObjectAction, ETriggerEvent::Canceled, this, &ADoorActorBase::GrabObjectCompletedHandler);
			bInputBinded = true;
		}
	}

	if (auto* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
	{
		if (DoorInputContext && !Subsystem->HasMappingContext(DoorInputContext))
		{
			// Добавляем контекст ввода двери
			FModifyContextOptions Options;
			Options.bIgnoreAllPressedKeysUntilRelease = false;
			Subsystem->AddMappingContext(DoorInputContext, DoorInputContextPriority, Options);
		}
	}
}

void ADoorActorBase::OnHoverEnd_Implementation(APlayerController* PlayerController)
{
	IInteractiveObject::OnHoverEnd_Implementation(PlayerController);
	DisableInput(PlayerController);
	DragPlayerController = nullptr;

	if (auto* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
	{
		if (DoorInputContext && Subsystem->HasMappingContext(DoorInputContext))
		{
			// Удаляем контекст ввода двери
			FModifyContextOptions Options;
			Options.bIgnoreAllPressedKeysUntilRelease = false;
			Subsystem->RemoveMappingContext(DoorInputContext, Options);
		}
	}
}

void ADoorActorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ADoorActorBase::DoorDragActionHandler(const FInputActionValue& ActionValue)
{
	if (!DragPlayerController) return;

	float Angle = DoorRootComponent->GetRelativeRotation().Yaw;
	Angle += ActionValue.Get<float>() * DragMagnitude;
	Angle = FMath::ClampAngle(Angle, MinDoorAngle, MaxDoorAngle);
	const FRotator NewRotation = FRotator(0.0f, Angle, 0.0f);
	DoorRootComponent->SetRelativeRotation(NewRotation, true);
}

void ADoorActorBase::OnDoorAssetLoaded(FPrimaryAssetId LoadedAssetID)
{
	if (const auto* Manager = UAssetManager::GetIfValid())
	{
		if (auto* DoorData = Cast<UDoorDataAsset>(Manager->GetPrimaryAssetObject(LoadedAssetID)))
		{
			InitFromAsset_Implementation(DoorData);
		}
	}
}

void ADoorActorBase::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (PropertyChangedEvent.MemberProperty && PropertyChangedEvent.MemberProperty->GetFName() == GET_MEMBER_NAME_CHECKED(ADoorActorBase, DoorID))
	{
		ReloadDoorAsset();
	}
}
