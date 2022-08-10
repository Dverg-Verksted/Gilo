// It is owned by the company Dverg Verksted.

#include "Game/MainMenu/MainMenuSpline.h"
#include "MainMenuCard.h"
#include "Components/SplineComponent.h"

AMainMenuSpline::AMainMenuSpline()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>("SceneRoot");
	RootComponent = SceneRoot;

	SplineComponent = CreateDefaultSubobject<USplineComponent>("SplineComponent");
	SplineComponent->SetupAttachment(GetRootComponent());

	bOpen = false;
}
//Запуск движения по сплайну карты по таймлайну FromStart- карта выезжает Revers - уезжает
void AMainMenuSpline::GoForSpline(bool bCardMove)
{
	bOpen = !bOpen;
	if (bOpen)
	{
		MoveValue = 1.0f;
		MyTimeLine.PlayFromStart();
	}
	else
	{
		MyTimeLine.Reverse();
	}
}

//Установка перемещения карты по timeline относительно значений timeline
void AMainMenuSpline::ControlCard()
{
	TimelineValue = MyTimeLine.GetPlaybackPosition();
	CurveFloatValue = MoveValue * CurveFloat->GetFloatValue(TimelineValue);

	FVector VectorMove = SplineComponent->GetLocationAtTime(CurveFloatValue, ESplineCoordinateSpace::World);

	FRotator RotatorCard = CardRef->GetActorRotation();
	FQuat NewRotation = FQuat(RotatorCard);

	NewTransform.SetLocation(VectorMove);
	NewTransform.SetRotation(NewRotation);

	CardRef->SetActorTransform(NewTransform);
}

void AMainMenuSpline::BeginPlay()
{
	Super::BeginPlay();
	AMainMenuCard* OnClickedCard = Cast<AMainMenuCard>(CardRef);
	if (OnClickedCard->IsValidLowLevel())
	{
		OnClickedCard->Click.AddDynamic(this, &AMainMenuSpline::GoForSpline);
	}
	MoveValue = 1.0f;

	if (!CurveFloat) return;

	TimelimeCallback.BindUFunction(this, FName("ControlCard"));

	MyTimeLine.AddInterpFloat(CurveFloat, TimelimeCallback);
	MyTimeLine.SetTimelineFinishedFunc(TimelineFinishedCallback);
}

void AMainMenuSpline::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	MyTimeLine.TickTimeline(DeltaTime);
}
