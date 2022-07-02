// It is owned by the company Dverg Verksted.


#include "Game/InteractionSystem/InteractiveObjects/DrawerActorBase.h"

#include "DataRegistrySubsystem.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/AssetManager.h"
#include "Game/Common/AssetMetaRegistrySource.h"
#include "Game/InteractionSystem/InteractionSettings.h"
#include "Game/InteractionSystem/DataAssets/DrawerDataAsset.h"

ADrawerActorBase::ADrawerActorBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this,TEXT("SceneRoot"));
	RootComponent = SceneRoot;

	DrawerRootComponent = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this,TEXT("DrawerRootComponent"));
	DrawerRootComponent->SetupAttachment(RootComponent);

	DrawerMeshComponent = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this,TEXT("DrawerMeshComponent"));
	DrawerMeshComponent->SetupAttachment(DrawerRootComponent);

	DragPlayerController = nullptr;
}

void ADrawerActorBase::BeginPlay()
{
	Super::BeginPlay();
}

void ADrawerActorBase::DrawerDragActionHandler(const FInputActionValue& ActionValue)
{
	if (!DragPlayerController)
		return;

	FVector NewLocation = DrawerRootComponent->GetRelativeLocation();
	float Extend = NewLocation.X;
	Extend += ActionValue.Get<float>() * DragMagnitude;
	Extend = FMath::Clamp(Extend, 0.0f, MaxExtendDistance);
	NewLocation.X = Extend;
	DrawerRootComponent->SetRelativeLocation(NewLocation, true);
	GEngine->AddOnScreenDebugMessage(156,0.35f, FColor::Emerald, NewLocation.ToCompactString());
}

void ADrawerActorBase::DrawerDragStopActionHandler(const FInputActionValue& ActionValue)
{
}

void ADrawerActorBase::OnDrawerAssetLoaded(FPrimaryAssetId LoadedAssetID)
{
	if (const auto* Manager = UAssetManager::GetIfValid())
	{
		if (auto* DoorData = Cast<UDrawerDataAsset>(Manager->GetPrimaryAssetObject(LoadedAssetID)))
		{
			InitFromAsset_Implementation(DoorData);
		}
	}
}

void ADrawerActorBase::ReloadDrawerAsset()
{
	const auto* RegistrySystem = UDataRegistrySubsystem::Get();
	if (IsValid(RegistrySystem))
	{
		if (const UDataRegistry* Registry = RegistrySystem->GetRegistryForType(DrawerID.RegistryType))
		{
			if (auto* Asset = Registry->GetCachedItem<FAssetMetaRegistryRow>(DrawerID))
			{
				if (UAssetManager* Manager = UAssetManager::GetIfValid())
				{
					TArray<FName> Bundles;
					Bundles.Add(FName("meshes"));
					const FStreamableDelegate Delegate = FStreamableDelegate::CreateUObject(this, &ADrawerActorBase::OnDrawerAssetLoaded,
						Asset->AssetID);
					Manager->LoadPrimaryAsset(Asset->AssetID, Bundles, Delegate);
				}
			}
		}
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
	}
}

void ADrawerActorBase::OnHoverBegin_Implementation(APlayerController* PlayerController, const FHitResult& Hit)
{
	IInteractiveObject::OnHoverBegin_Implementation(PlayerController, Hit);
	EnableInput(PlayerController);
	DragPlayerController = PlayerController;

	if (!bInputBinded)
	{
		// Привязка к вводу
		if (auto* EIC = Cast<UEnhancedInputComponent>(InputComponent))
		{
			ensure(DrawerDragAction);

			if (DrawerDragAction)
			{
				EIC->BindAction(DrawerDragAction, ETriggerEvent::Triggered, this, &ADrawerActorBase::DrawerDragActionHandler);
				EIC->BindAction(DrawerDragAction, ETriggerEvent::Completed, this, &ADrawerActorBase::DrawerDragStopActionHandler);
				EIC->BindAction(DrawerDragAction, ETriggerEvent::Canceled, this, &ADrawerActorBase::DrawerDragStopActionHandler);
				bInputBinded = true;
			}
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

void ADrawerActorBase::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (PropertyChangedEvent.MemberProperty && PropertyChangedEvent.MemberProperty->GetFName() == GET_MEMBER_NAME_CHECKED(ADrawerActorBase,
		    DrawerID))
	{
		ReloadDrawerAsset();
	}
}
