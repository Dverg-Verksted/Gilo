// It is owned by the company Dverg Verksted.


#include "Game/InteractionSystem/InteractionListenerComponent.h"

UInteractionListenerComponent::UInteractionListenerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInteractionListenerComponent::OnHoverBegin_Implementation(APlayerController* PlayerController, const FHitResult& Hit)
{
	OnOwnerHoverBegin.Broadcast(PlayerController, Hit);
}

void UInteractionListenerComponent::OnHoverEnd_Implementation(APlayerController* PlayerController)
{
	OnOwnerHoverEnd.Broadcast(PlayerController);
}

void UInteractionListenerComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UInteractionListenerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
