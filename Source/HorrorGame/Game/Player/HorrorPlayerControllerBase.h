// It is owned by the company Dverg Verksted.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/Actor.h"
#include "HorrorPlayerControllerBase.generated.h"

UCLASS(Abstract, BlueprintType)
class HORRORGAME_API AHorrorPlayerControllerBase : public APlayerController, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	AHorrorPlayerControllerBase(const FObjectInitializer& ObjectInitializer);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//----------------------------------------------------------------------//
	// IGenericTeamAgentInterface
	//----------------------------------------------------------------------//
private:
	FGenericTeamId TeamID;

public:
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override { return TeamID; }
};
