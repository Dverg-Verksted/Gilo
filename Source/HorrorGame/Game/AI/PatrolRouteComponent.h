// It is owned by the company Dverg Verksted.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/SplineComponent.h"
#include "PatrolRouteComponent.generated.h"

UCLASS(NotBlueprintable, BlueprintType, ClassGroup = AI, meta = (BlueprintSpawnableComponent))
class HORRORGAME_API UPatrolRouteComponent : public USplineComponent
{
	GENERATED_BODY()

public:
	UPatrolRouteComponent();

protected:
	virtual void BeginPlay() override;

public:
	/* Возвращает количество точек маршрута */
	UFUNCTION(BlueprintPure)
	int32 GetRoutePointsNum() const;

	/* Возвращает координаты точки маршрута */
	UFUNCTION(BlueprintPure)
	void GetRoutePointLocation(bool& bSuccess, const int32& Index, FVector& Location) const;

	/* Возвращает ближайшую точку маршрута */
	UFUNCTION(BlueprintPure)
	void GetClosestPointToLocation(const FVector& CheckLocation, int32& ClosestIndex, FVector& ClosestLocation, double& ClosestDistance) const;
};
