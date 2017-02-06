// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/TargetPoint.h"
#include "PathWayPoint.generated.h"

/**
 * 
 */
UCLASS()
class INSECTION2DAPPROACH_API APathWayPoint : public ATargetPoint
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Pathfinding")
        TArray<APathWayPoint*> ReachableWaypoints;
	
    UFUNCTION(BlueprintCallable, Category = "AI|Pathginding")
    void FindReachableWayPointsFromArray(UPARAM(ref) const TArray<APathWayPoint*>& WayPointPath);
};
