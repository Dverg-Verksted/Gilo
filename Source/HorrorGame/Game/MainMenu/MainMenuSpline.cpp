// It is owned by the company Dverg Verksted.


#include "Game/MainMenu/MainMenuSpline.h"
#include "Components/SplineComponent.h"

AMainMenuSpline::AMainMenuSpline()
{
	PrimaryActorTick.bCanEverTick = true;
	
	SceneRoot = CreateDefaultSubobject<USceneComponent>("SceneRoot");
	RootComponent = SceneRoot;

	SplineComponent = CreateDefaultSubobject<USplineComponent>("SplineComponent");
	SplineComponent->SetupAttachment(GetRootComponent());

	bOpen = false;
	bReadyState = true;
}
//Запуск движения по сплайну карты по таймлайну FromStart- карта выезжает Revers - уезжает
void AMainMenuSpline::GoForSpline(bool bCardMove) 
{
	if (bReadyState)
	{
		bOpen = !bOpen;
		if (bOpen)
		{
			MoveValue = 1.0f;
			bReadyState = false;
			MyTimeLine.PlayFromStart();
		}
		else
		{
			bReadyState = false;
			MyTimeLine.Reverse();
		}
	}
}
//Установка текущего значения готовности к передвижению по сплайну
void AMainMenuSpline::SetState() 
{
	bReadyState = true;
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
	AMainMenuCard* OnClickedCard = Cast<AMainMenuCard>(CardRef);
	if (OnClickedCard->IsValidLowLevel())
	{
		OnClickedCard->Click.AddDynamic(this, &AMainMenuSpline::GoForSpline);
	}
	MoveValue = 1.0f;

	if (CurveFloat)
	{
		FOnTimelineFloat TimelimeCallback;
		FOnTimelineEventStatic TimelineFinishedCallback;

		TimelimeCallback.BindUFunction(this, FName("ControlCard"));
		TimelineFinishedCallback.BindUFunction(this, FName("SetState"));

		MyTimeLine.AddInterpFloat(CurveFloat, TimelimeCallback);
		MyTimeLine.SetTimelineFinishedFunc(TimelineFinishedCallback);
	}
	Super::BeginPlay();
	
}

void AMainMenuSpline::Tick(float DeltaTime)
{
	MyTimeLine.TickTimeline(DeltaTime);
	Super::Tick(DeltaTime);
}

