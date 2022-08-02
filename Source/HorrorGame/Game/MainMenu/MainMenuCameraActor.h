// It is owned by the company Dverg Verksted.

#pragma once

#include "CoreMinimal.h"
#include "MainMenuCard.h"
#include "MainMenuSpline.h"
#include "GameFramework/Actor.h"
#include "Camera/CameraComponent.h"
#include "MainMenuCameraActor.generated.h"

UCLASS()
class HORRORGAME_API AMainMenuCameraActor : public AActor
{
	GENERATED_BODY()

public:
	AMainMenuCameraActor();

	UPROPERTY(EditAnywhere, Category = "Card")
	AMainMenuCard* CardRef;
	UPROPERTY(EditAnywhere, Category = "Card")
	AMainMenuSpline* SplineRef;

	TArray<AActor*> CardActors;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USceneComponent> SceneRoot;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UCameraComponent> CameraComponent;

	UFUNCTION()
	void SetCameraMod(bool bCardMove);

	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
};
