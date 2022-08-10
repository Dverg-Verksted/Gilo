// It is owned by the company Dverg Verksted.

#include "Game/MainMenu/MainMenuCard.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TimelineComponent.h"
#include "Components/SceneComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"

AMainMenuCard::AMainMenuCard()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>("SceneRoot");
	RootComponent = SceneRoot;

	CardMesh = CreateDefaultSubobject<UStaticMeshComponent>("CardMesh");
	CardMesh->SetupAttachment(GetRootComponent());

	MainCamera = CreateDefaultSubobject<UCameraComponent>("MainCamera");
	MainCamera->SetupAttachment(GetRootComponent());

	OnClicked.AddUniqueDynamic(this, &AMainMenuCard::OnSelected);

	bOpen = false;
}
//Запуск поворота карты по таймлайну FromStart- карта выезжает Revers - уезжает
void AMainMenuCard::ToggleCard()
{
	FTimerHandle CardMoveDelayTimerHandle;
	bCardMove = true;
	GetWorld()->GetTimerManager().SetTimer(CardMoveDelayTimerHandle, this, &AMainMenuCard::CardMoveFinished, 2.0f, false);

	bOpen = !bOpen;
	CardRotation = CardMesh->GetRelativeRotation();
	Click.Broadcast(bOpen);
	if (bOpen)
	{
		RotateValue = 1.0f;
		MyTimeLine.PlayFromStart();
	}
	else
	{
		MyTimeLine.Reverse();
	}
}
//Установка вращения карты относительно значений timeline
void AMainMenuCard::ControlCard()
{
	TimelineValue = MyTimeLine.GetPlaybackPosition();
	CurveFloatValue = RotateValue * CurveFloat->GetFloatValue(TimelineValue);

	FQuat NewRotation = FQuat(FRotator(CurveFloatValue, 0.0f, 0.0f));
	CardMesh->SetRelativeRotation(NewRotation);
}

void AMainMenuCard::CardMoveFinished()
{
	bCardMove = false;
}

//Действие по клику, проверяем есть ли другие открытые карты чтобы не тыкать много карт
void AMainMenuCard::OnSelected(AActor* Target, FKey ButtonPressed)
{
	int32 CountCards = 0;

	if (!this->bCardMove)
	{
		for (auto* Actor : CardActors)
		{
			if (auto* CardActor = Cast<AMainMenuCard>(Actor))
			{
				if (CardActor->bCardMove) CountCards++;
			}
		}

		if (CountCards == 0) ToggleCard();
	}
}

void AMainMenuCard::BeginPlay()
{
	Super::BeginPlay();
	UGameplayStatics::GetAllActorsOfClass(this, AMainMenuCard::StaticClass(), CardActors);
	RotateValue = 1.0f;

	if (!CurveFloat) return;

	FOnTimelineFloat TimelimeCallback;
	FOnTimelineEventStatic TimelineFinishedCallback;

	TimelimeCallback.BindUFunction(this, FName("ControlCard"));

	MyTimeLine.AddInterpFloat(CurveFloat, TimelimeCallback);
	MyTimeLine.SetTimelineFinishedFunc(TimelineFinishedCallback);
}

void AMainMenuCard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	MyTimeLine.TickTimeline(DeltaTime);
}
