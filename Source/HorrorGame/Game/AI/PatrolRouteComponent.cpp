// It is owned by the company Dverg Verksted.

#include "PatrolRouteComponent.h"

UPatrolRouteComponent::UPatrolRouteComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	bHiddenInGame = true;
}

void UPatrolRouteComponent::BeginPlay()
{
	Super::BeginPlay();
}

int32 UPatrolRouteComponent::GetRoutePointsNum() const
{
	return GetNumberOfSplinePoints();
}

void UPatrolRouteComponent::GetRoutePointLocation(bool& bSuccess, const int32& Index, FVector& Location) const
{
	bSuccess = false;
	const int32 TotalPoints = GetRoutePointsNum();
	if (Index < 0 || Index >= TotalPoints) return;

	Location = GetLocationAtSplinePoint(Index, ESplineCoordinateSpace::World);
}

void UPatrolRouteComponent::GetClosestPointToLocation(const FVector& CheckLocation, int32& ClosestIndex, FVector& ClosestLocation, double& ClosestDistance) const
{
	ClosestDistance = 0.0f;
	for (int32 Index = 0, Num = GetRoutePointsNum(); Index < Num; Index++)
	{
		FVector PointLocation = GetLocationAtSplinePoint(Index, ESplineCoordinateSpace::World);
		const double Distance = FVector::Distance(CheckLocation, PointLocation);
		if (ClosestDistance == 0.0f || Distance < ClosestDistance)
		{
			ClosestIndex = Index;
			ClosestLocation = PointLocation;
			ClosestDistance = Distance;
		}
	}
}
