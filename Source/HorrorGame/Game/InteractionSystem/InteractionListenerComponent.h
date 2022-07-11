// It is owned by the company Dverg Verksted.

#pragma once

#include "CoreMinimal.h"
#include "InteractiveObject.h"
#include "Components/ActorComponent.h"
#include "InteractionListenerComponent.generated.h"

/* Компонент реагирует на события взаимодействия с родительским актором */
UCLASS(Abstract, BlueprintType, Blueprintable)
class HORRORGAME_API UInteractionListenerComponent : public UActorComponent, public IInteractiveObject
{
	GENERATED_BODY()

public:
	UInteractionListenerComponent();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnOwnerHoverBeginEvent, APlayerController*, PlayerController, FHitResult, Hit);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnOwnerHoverEndEvent, APlayerController*, PlayerController);

	/* Навели прицел на интерактивный объект, которому принадлежит текущий компонент */
	UPROPERTY(BlueprintAssignable)
	FOnOwnerHoverBeginEvent OnOwnerHoverBegin;

	/* Убрали прицел с интерактивного объекта, которому принадлежит текущий компонент */
	UPROPERTY(BlueprintAssignable)
	FOnOwnerHoverEndEvent OnOwnerHoverEnd;

	// Interactive object interface BEGIN
	virtual void OnHoverBegin_Implementation(APlayerController* PlayerController, const FHitResult& Hit) override;
	virtual void OnHoverEnd_Implementation(APlayerController* PlayerController) override;
	// Interactive object interface END

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
