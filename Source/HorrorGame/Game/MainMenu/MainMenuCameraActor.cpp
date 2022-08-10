// It is owned by the company Dverg Verksted.

#include "Game/MainMenu/MainMenuCameraActor.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/SingleThreadEvent.h"
#include "Camera/CameraModifier.h"

AMainMenuCameraActor::AMainMenuCameraActor()
{
	PrimaryActorTick.bCanEverTick = true;
	SceneRoot = CreateDefaultSubobject<USceneComponent>("SceneRoot");
	RootComponent = SceneRoot;
	CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	CameraComponent->SetupAttachment(GetRootComponent());
}

void AMainMenuCameraActor::SetCameraMod(bool bCardMove)
{
	APlayerController* OurPlayer = UGameplayStatics::GetPlayerController(this, 0);
	//ищем конкретную карту на которую кликнули
	for (auto* Actor : CardActors)
	{
		if (auto* CardActor = Cast<AMainMenuCard>(Actor))
		{
			if (CardActor->bOpen) CardRef = CardActor;
		}
	}
	//устанавливаем фокус на объект который кликнули
	if (bCardMove)
	{
		OurPlayer->SetViewTargetWithBlend(CardRef, 2.0f, BlendFunc, 2.0f, false);
	}
	else
	{
		OurPlayer->SetViewTargetWithBlend(this, 2.0f, BlendFunc, 2.0f, false);
	}
}

void AMainMenuCameraActor::BeginPlay()
{
	Super::BeginPlay();
	//получаем все экземпляры класса карты
	UGameplayStatics::GetAllActorsOfClass(this, AMainMenuCard::StaticClass(), CardActors);
	for (auto* Actor : CardActors)
	{
		if (auto* CardActor = Cast<AMainMenuCard>(Actor))
		{
			CardActor->Click.AddDynamic(this, &AMainMenuCameraActor::SetCameraMod);
		}
	}

	FViewTargetTransitionParams TransitionParams = FViewTargetTransitionParams();
	GetWorld()->GetFirstPlayerController()->SetViewTarget(this, TransitionParams);
}

void AMainMenuCameraActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
