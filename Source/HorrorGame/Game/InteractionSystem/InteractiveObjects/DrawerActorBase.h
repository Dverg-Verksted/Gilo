// It is owned by the company Dverg Verksted.

#pragma once

#include "CoreMinimal.h"
#include "DataRegistryId.h"
#include "GameplayTagAssetInterface.h"
#include "InputAction.h"
#include "Game/InteractionSystem/InteractiveObject.h"
#include "GameFramework/Actor.h"
#include "DrawerActorBase.generated.h"

UCLASS()
class HORRORGAME_API ADrawerActorBase
	: public AActor,
	  public IInteractiveObject,
	  public IGameplayTagAssetInterface
{
	GENERATED_BODY()

public:
	ADrawerActorBase(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USceneComponent> DrawerRootComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> DrawerMeshComponent;

	/* Дата Ассет ящика */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Drawer")
	FDataRegistryId DrawerID;

	/* Максимальная дистанция выдвижения ящика */
	UPROPERTY(EditDefaultsOnly, Category="Drawer")
	float MaxExtendDistance = 45.0f;

	/* Коэффициент выдвижения */
	UPROPERTY(EditDefaultsOnly, Category="Drawer")
	float DragMagnitude = 10.0f;
	
	/* Действие открытия ящика */
	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> DrawerDragAction;

	/* Продолжительное действие */
	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> GrabObjectAction;
	
	/* Контекст действий с ящиком */
	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputMappingContext> DrawerInputContext;

	/* Приоритет контекста управления ящиком */
	UPROPERTY(EditDefaultsOnly, Category="Input")
	int32 DrawerInputContextPriority = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameplayTags",
		Meta = (DisplayName = "GameplayTags", ExposeOnSpawn = true), SaveGame)
	FGameplayTagContainer GameplayTags;
	
protected:
	/* Требуемый угол открытия двери */
	float DesiredAngle = 0.0f;

	UPROPERTY()
	TObjectPtr<APlayerController> DragPlayerController;
	
	virtual void BeginPlay() override;

	UFUNCTION()
	void DrawerDragActionHandler(const FInputActionValue& ActionValue);

	UFUNCTION()
	void GrabObjectTriggeredHandler(const FInputActionValue& ActionValue);

	UFUNCTION()
	void GrabObjectCompletedHandler(const FInputActionValue& ActionValue);

	UFUNCTION()
	void OnDrawerAssetLoaded(FPrimaryAssetId LoadedAssetID);

	void ReloadDrawerAsset();
	
public:
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void Tick(float DeltaTime) override;

	// Interactive object interface BEGIN
	virtual void InitFromAsset_Implementation(UPrimaryDataAsset* SourceAsset) override;
	virtual void OnHoverBegin_Implementation(APlayerController* PlayerController, const FHitResult& Hit) override;
	virtual void OnHoverEnd_Implementation(APlayerController* PlayerController) override;
	// Interactive object interface END

	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override
	{
		TagContainer = GameplayTags;
	}

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
private:
	/* TRUE - Если уже привязались к вводу */
	bool bInputBinded = false;
	/* TRUE - Если двигаем ящик */
	bool bDragged = false;
};
