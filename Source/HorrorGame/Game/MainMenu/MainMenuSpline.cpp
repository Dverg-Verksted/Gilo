// It is owned by the company Dverg Verksted.


#include "Game/MainMenu/MainMenuSpline.h"
#include "Components/SplineComponent.h"

// Sets default values
AMainMenuSpline::AMainMenuSpline()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	SceneRoot = CreateDefaultSubobject<USceneComponent>("SceneRoot");
	RootComponent = SceneRoot;

	SplineComponent = CreateDefaultSubobject<USplineComponent>("SplineComponent");
	SplineComponent->SetupAttachment(GetRootComponent());

	Open = false;
	ReadyState = true;
}

void AMainMenuSpline::GoForSpline(bool IsOpen) 
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString("OnClick"));
	if (ReadyState)
	{
		Open = !Open;
		//CardRotation = CardMesh->GetRelativeRotation();
		if (Open)
		{
			MoveValue = 1.0f;
			ReadyState = false;
			MyTimeLine.PlayFromStart();
		}
		else
		{
			ReadyState = false;
			MyTimeLine.Reverse();
		}
	}
}

void AMainMenuSpline::SetState() 
{
	ReadyState = true;
}

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

// Called when the game starts or when spawned
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

// Called every frame
void AMainMenuSpline::Tick(float DeltaTime)
{
	MyTimeLine.TickTimeline(DeltaTime);
	Super::Tick(DeltaTime);
}

