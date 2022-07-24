// It is owned by the company Dverg Verksted.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MainMenuCard.h"
#include "Components/TimelineComponent.h"
#include "MainMenuSpline.generated.h"

class USplineComponent;
class UCurveFloat;

UCLASS()
class HORRORGAME_API AMainMenuSpline : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMainMenuSpline();
	UPROPERTY(EditAnywhere, Category = "Timeline")
	AMainMenuCard* CardRef;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USceneComponent> SceneRoot;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<USplineComponent> SplineComponent;
	UPROPERTY(EditAnywhere, Category = "Timeline")
	TObjectPtr<UCurveFloat> CurveFloat;

	UFUNCTION()
	void GoForSpline(bool bCardMove);
	UFUNCTION()
	void SetState();
	UFUNCTION()
	void ControlCard();
	

	virtual void BeginPlay() override;

public:	
	
	bool bOpen; //приехала карта или нет
	bool bReadyState; 
	float MoveValue; // на сколько пододвинуть за тик
	float CurveFloatValue; //текущее значение кривой
	float TimelineValue;
	FRotator CardRotation;
	FTransform NewTransform;
	FTimeline MyTimeLine;

	virtual void Tick(float DeltaTime) override;

};
