// It is owned by the company Dverg Verksted.


#include "Game/MainMenu/MainMenuCard.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TimelineComponent.h"
#include "Components/SceneComponent.h"
#include "Camera/CameraComponent.h"

AMainMenuCard::AMainMenuCard()
{
	PrimaryActorTick.bCanEverTick = true;
	
	SceneRoot = CreateDefaultSubobject<USceneComponent>("SceneRoot");
	RootComponent = SceneRoot;

	CardMesh = CreateDefaultSubobject<UStaticMeshComponent>("CardMesh");
	CardMesh->SetupAttachment(GetRootComponent());

	CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	CameraComponent->SetupAttachment(GetRootComponent());

	OnClicked.AddUniqueDynamic(this, &AMainMenuCard::OnSelected);

	bOpen = false;
	bReadyState = true;
}
//Запуск поворота карты по таймлайну FromStart- карта выезжает Revers - уезжает
void AMainMenuCard::ToggleCard()
{
	if (bReadyState)
	{
		bOpen = !bOpen;
		CardRotation = CardMesh->GetRelativeRotation();
		if (bOpen)
		{
			RotateValue = 1.0f;
			bReadyState = false;
			MyTimeLine.PlayFromStart();
			//сюда функция приближения камеры
		}
		else
		{
			bReadyState = false;
			MyTimeLine.Reverse();

			//сюда функция возврата камеры
		}
		
	}
		
}
//Установка вращения карты относительно значений timeline
void AMainMenuCard::ControlCard() 
{
	TimelineValue = MyTimeLine.GetPlaybackPosition();
	CurveFloatValue = RotateValue * CurveFloat->GetFloatValue(TimelineValue);

	FQuat NewRotation = FQuat(FRotator(CurveFloatValue,0.0f, 0.0f));
	CardMesh->SetRelativeRotation(NewRotation);
}
//Установка текущего значения готовности к использованию карты
void AMainMenuCard::SetState() 
{
	bReadyState = true;
}

void AMainMenuCard::OnSelected(AActor* Target, FKey ButtonPressed)
{
	Click.Broadcast(true);
	ToggleCard();
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan, FString("flip"));
}

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

void AMainMenuCard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	MyTimeLine.TickTimeline(DeltaTime);
}

