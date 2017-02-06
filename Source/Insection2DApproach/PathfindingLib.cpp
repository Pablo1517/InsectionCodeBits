// Fill out your copyright notice in the Description page of Project Settings.

#include "Insection2DApproach.h"
#include "PathfindingLib.h"
#include "PathWayPoint.h"
#include "Commons/PathfindingCommons.h"


bool UPathfindingLib::FindWaypointPathImpl(const FVector &startLocation, APathWayPoint* TargetWayPoint, UPARAM(ref) TArray<APathWayPoint*>& WayPointPath, TArray<APathWayPoint*>& alreadyVisitedWaypoints)
{
    if (!TargetWayPoint)
        return false;

    FVector TargetLocation = TargetWayPoint->GetActorLocation();
    
    FCollisionQueryParams traceParams = FCollisionQueryParams(FName(TEXT("PathfindingTrace")), true);
    traceParams.bTraceComplex = true;
    traceParams.bTraceAsyncScene = false;
    traceParams.bReturnPhysicalMaterial = false;

    //Re-initialize hit info
    FHitResult TraceHit(ForceInit);
    
    //call GetWorld() from within an actor extending class
    UWorld* currentWorld = TargetWayPoint->GetWorld();
    if (!currentWorld)
    {
        LogToScreen("ERROR - UPathfindingLib::FindWaypointPathImpl - Failed to get UWorld!");
        return false;
    }
    currentWorld->LineTraceSingleByChannel(TraceHit, startLocation, TargetLocation, TRACE_CHANNEL_ENEMIS, traceParams);

    LogToScreen(FString::Printf(TEXT("Pathfinding going through %s"), *TargetWayPoint->GetName()), 3.0f);
    alreadyVisitedWaypoints.Add(TargetWayPoint);

    // If trace was NOT blocked by terrain, that means we can get to this waypoint
    if (!TraceHit.bBlockingHit)
    {
        WayPointPath.Add(TargetWayPoint);
        return true;
    }

    TraceHit.Init();

    bool foundValidPath = false;

    for (APathWayPoint* wayPoint : TargetWayPoint->ReachableWaypoints)
    {
        if (wayPoint && !alreadyVisitedWaypoints.Contains(wayPoint))
        {
            foundValidPath = FindWaypointPathImpl(startLocation, wayPoint, WayPointPath, alreadyVisitedWaypoints);
            if (foundValidPath)
            {
                WayPointPath.Add(TargetWayPoint);
                break;
            }
        }
    }
        
    return foundValidPath;
}

bool UPathfindingLib::FindWaypointPath(FVector startLocation, APathWayPoint* TargetWayPoint, UPARAM(ref) TArray<APathWayPoint*>& WayPointPath)
{
    // array of alraedy visited waypoints so that we don't loop till the end of world
    TArray<APathWayPoint*> alreadyVisitedWaypoints;

    return FindWaypointPathImpl(startLocation, TargetWayPoint, WayPointPath, alreadyVisitedWaypoints);
}