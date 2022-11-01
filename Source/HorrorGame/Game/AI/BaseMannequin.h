// It is owned by the company Dverg Verksted.

#pragma once

#include "CoreMinimal.h"
#include "Game/Common/AssetSetupActor.h"
#include "GameFramework/Character.h"
#include "Perception/AIPerceptionComponent.h"
#include "BaseMannequin.generated.h"

UCLASS(Abstract, BlueprintType)
class HORRORGAME_API ABaseMannequin : public ACharacter, public IAssetSetupActor
{
	GENERATED_BODY()

public:
	ABaseMannequin(const FObjectInitializer& ObjectInitializer);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UAIPerceptionComponent> PerceptionComponent;

	/* Скорость ходьбы */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float WalkSpeed = 128.0f;

	/* Скорость бега */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float RunSpeed = 300.0f;

	virtual void BeginPlay() override;

public:
	virtual void InitFromAsset_Implementation(const UPrimaryDataAsset* SourceAsset) override;
	virtual void Tick(float DeltaTime) override;
};
