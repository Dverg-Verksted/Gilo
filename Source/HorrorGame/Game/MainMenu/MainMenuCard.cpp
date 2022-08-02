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
//������ �������� ����� �� ��������� FromStart- ����� �������� Revers - �������
void AMainMenuCard::ToggleCard()
{
	if (this->bOpen)
	{
		
	}
	else
	{
		bOpen = !bOpen;
	}
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
//��������� �������� ����� ������������ �������� timeline
void AMainMenuCard::ControlCard()
{
	TimelineValue = MyTimeLine.GetPlaybackPosition();
	CurveFloatValue = RotateValue * CurveFloat->GetFloatValue(TimelineValue);

	FQuat NewRotation = FQuat(FRotator(CurveFloatValue, 0.0f, 0.0f));
	CardMesh->SetRelativeRotation(NewRotation);
}

//�������� �� �����, ��������� ���� �� ������ �������� ����� ����� �� ������ ����� ����
void AMainMenuCard::OnSelected(AActor* Target, FKey ButtonPressed)
{
	int32 CountCards = 0;
	if (this->bOpen)
	{
		ToggleCard();
	}
	else
	{
		for (auto* Actor : CardActors)
		{
			if (auto* CardActor = Cast<AMainMenuCard>(Actor))
			{
				if (CardActor->bOpen) CountCards++;
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
