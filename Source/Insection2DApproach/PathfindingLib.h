// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "PathfindingLib.generated.h"

/**
 * 
 */
UCLASS()
class INSECTION2DAPPROACH_API UPathfindingLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
	
public:
        static bool FindWaypointPathImpl(const FVector &startLocation, APathWayPoint* TargetWayPoint, UPARAM(ref) TArray<APathWayPoint*>& WayPointPath, TArray<APathWayPoint*>& alreadyVisitedWaypoints);

        /** Finds an array of waypoints as path and. WARNING: This function modifies WayPointPath parameter as it is passed by reference! */
        UFUNCTION(BlueprintCallable, Category = "AI|Pathginding")
        static bool FindWaypointPath(FVector startLocation, APathWayPoint* TargetWayPoint, UPARAM(ref) TArray<APathWayPoint*>& WayPointPath);
	
};
