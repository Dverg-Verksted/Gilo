// It is owned by the company Dverg Verksted.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GameFramework/Actor.h"
#include "BaseMannequinController.generated.h"

UCLASS(BlueprintType)
class HORRORGAME_API ABaseMannequinController : public AAIController
{
	GENERATED_BODY()

public:
	ABaseMannequinController(const FObjectInitializer& ObjectInitializer);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
