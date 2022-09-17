// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Perception/AIPerceptionComponent.h"
#include "AIController.h"
#include "AITempController.generated.h"

/**
 * 
 */
UCLASS()
class UE5TESTPROJECT_API AAITempController : public AAIController
{
	GENERATED_BODY()
protected:
	UPROPERTY(VisibleAnywhere)
	UAIPerceptionComponent* AIPerceptionComponent;

	virtual void BeginPlay();

public:

	AAITempController(const FObjectInitializer &ObjectInitializer);

	UPROPERTY(BlueprintReadWrite)
	class AAngelBot* Agent;

	UFUNCTION()
	void OnPerception(AActor* Actor, FAIStimulus Stimulus);

	class UAISenseConfig_Sight* Sight;

	virtual void OnPossess(APawn* InPawn) override;

	virtual FRotator GetControlRotation() const override;
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION()
	void OnPawnDetected(const TArray<AActor*> &DetectedPawns);

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "AI")
	float ISightRadius = 2000.0f;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "AI")
	float ISightAge = 5.0f;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "AI")
	float ILoseSightRadius = ISightRadius + 500.0f;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "AI")
	float IFieldOfView = 90.0f;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category = "AI")
	bool bIsPlayerDetected = false;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category = "AI")
	float DistanceToPlayer = 0.0f;
	
};
