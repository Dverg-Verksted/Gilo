// It is owned by the company Dverg Verksted.


#include "Game/MainMenu/MainMenuCard.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TimelineComponent.h"
#include "Components/SceneComponent.h"
#include "Camera/CameraComponent.h"

// Sets default values
AMainMenuCard::AMainMenuCard()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	SceneRoot = CreateDefaultSubobject<USceneComponent>("SceneRoot");
	RootComponent = SceneRoot;

	CardMesh = CreateDefaultSubobject<UStaticMeshComponent>("CardMesh");
	CardMesh->SetupAttachment(GetRootComponent());

	CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	CameraComponent->SetupAttachment(GetRootComponent());

	OnClicked.AddUniqueDynamic(this, &AMainMenuCard::OnSelected);

	Open = false;
	ReadyState = true;
}
void AMainMenuCard::ToggleCard()
{
	if (ReadyState)
	{
		Open = !Open;
		CardRotation = CardMesh->GetRelativeRotation();
		if (Open)
		{
			RotateValue = 1.0f;
			ReadyState = false;
			MyTimeLine.PlayFromStart();
			//сюда функция приближения камеры
		}
		else
		{
			ReadyState = false;
			MyTimeLine.Reverse();

			//сюда функция возврата камеры
		}
		
	}
		
}

void AMainMenuCard::ControlCard() 
{
	TimelineValue = MyTimeLine.GetPlaybackPosition();
	CurveFloatValue = RotateValue * CurveFloat->GetFloatValue(TimelineValue);

	FQuat NewRotation = FQuat(FRotator(CurveFloatValue,0.0f, 0.0f));
	CardMesh->SetRelativeRotation(NewRotation);
}

void AMainMenuCard::SetState() 
{
	ReadyState = true;
}



void AMainMenuCard::OnSelected(AActor* Target, FKey ButtonPressed)
{
	Click.Broadcast(true);
	ToggleCard();
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan, FString("flip"));
}
// Called when the game starts or when spawned
void AMainMenuCard::BeginPlay()
{
	Super::BeginPlay();
	RotateValue = 1.0f;

	if (CurveFloat)
	{
		FOnTimelineFloat TimelimeCallback;
		FOnTimelineEventStatic TimelineFinishedCallback;

		TimelimeCallback.BindUFunction(this, FName("ControlCard"));
		TimelineFinishedCallback.BindUFunction(this, FName("SetState"));

		MyTimeLine.AddInterpFloat(CurveFloat, TimelimeCallback);
		MyTimeLine.SetTimelineFinishedFunc(TimelineFinishedCallback);
	}
}

// Called every frame
void AMainMenuCard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	MyTimeLine.TickTimeline(DeltaTime);
}

