// It is owned by the company Dverg Verksted.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
#include "MainMenuCard.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnClick, bool, IsOpen);

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
	
protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USceneComponent> SceneRoot;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UCameraComponent* CameraComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* CardMesh;
	UPROPERTY(EditAnywhere, Category = "Timeline")
	UCurveFloat* CurveFloat;
	
	

	UFUNCTION()
	void OnSelected(AActor* Target, FKey ButtonPressed);

	UFUNCTION()
	void ControlCard();
	UFUNCTION()
	void SetState();
	UFUNCTION()
	void ToggleCard();
	
	
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	bool Open;
	bool ReadyState;
	float RotateValue;
	float CurveFloatValue;
	float TimelineValue;
	FRotator CardRotation;
	FTimeline MyTimeLine;
};
