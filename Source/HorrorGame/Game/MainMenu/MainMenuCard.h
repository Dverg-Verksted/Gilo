// It is owned by the company Dverg Verksted.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
#include "MainMenuCard.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnClick, bool, bCardClick);

class UCameraComponent;
class UStaticMeshComponent;
class UCurveFloat;

UCLASS()
class HORRORGAME_API AMainMenuCard : public AActor
{
	GENERATED_BODY()

public:
	AMainMenuCard();

	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers")  // delegates to onclickevents
	FOnClick Click;

	UFUNCTION()
	void ToggleCard();
	UPROPERTY()
	TArray<AActor*> CardActors;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USceneComponent> SceneRoot;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<UStaticMeshComponent> CardMesh;
	UPROPERTY(EditAnywhere, Category = "Timeline")
	TObjectPtr<UCurveFloat> CurveFloat;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UCameraComponent> MainCamera;

	UFUNCTION()
	void OnSelected(AActor* Target, FKey ButtonPressed);
	UFUNCTION()
	void ControlCard();
	UFUNCTION()
	void CardMoveFinished();

	bool bCardMove = false;
	float RotateValue;
	float CurveFloatValue;
	float TimelineValue;
	FRotator CardRotation;
	FTimeline MyTimeLine;

	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	bool bOpen;
};
