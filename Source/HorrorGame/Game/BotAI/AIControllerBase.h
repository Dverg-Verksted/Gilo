// It is owned by the company Dverg Verksted.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Animation/AnimMontage.h"
#include "Perception/AIPerceptionTypes.h"
#include "AIControllerBase.generated.h"

/**
 * 
 */
UCLASS()
class HORRORGAME_API AAIControllerBase : public AAIController
{
	GENERATED_BODY()

public:
	
	AAIControllerBase(const FObjectInitializer &ObjectInitializer);

	class UAISenseConfig_Sight* Sight;

	UPROPERTY(BlueprintReadWrite)
	class ABotBase* Bot;

	UFUNCTION()
	void OnPerception(AActor* Actor, FAIStimulus Stimulus);

	virtual void OnPossess(APawn* InPawn) override;

	virtual FRotator GetControlRotation() const override;

	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable)
	void CharacterIsSee(bool bStay);

//	UFUNCTION()
//	void OnPawnDetected(const TArray<AActor*> &DetectedPawns);

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "AI")
	float ISightRadius;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "AI")
	float ISightAge;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "AI")
	float ILoseSightRadius = ISightRadius + 500.0f;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "AI")
	float IFieldOfView;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category = "AI")
	bool bIsPlayerDetected;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category = "AI")
	float DistanceToPlayer;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "AI")
	UAnimMontage* Montage;

protected:

	virtual void BeginPlay() override;

	class APlayerCharacterBase* ActorBase;

	UPROPERTY(VisibleAnywhere)
	UAIPerceptionComponent* AIPerceptionComponent;

	bool bStimulusSensed;
	bool bIsStay;

private:
	
};
