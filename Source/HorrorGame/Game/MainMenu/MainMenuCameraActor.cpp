// It is owned by the company Dverg Verksted.


#include "Game/MainMenu/MainMenuCameraActor.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
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
	EViewTargetBlendFunction BlendFunc = VTBlend_EaseInOut; 
	//���� ���������� ����� �� ������� ��������
	for (auto* Actor : CardActors)
	{
		if (auto* CardActor = Cast<AMainMenuCard>(Actor))
		{
			if (CardActor->bOpen) CardRef = CardActor;
		}
	}
	//������������� ����� �� ������ ������� ��������
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
	//�������� ��� ���������� ������ �����
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

	Super::BeginPlay();
	
}

void AMainMenuCameraActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
