// Fill out your copyright notice in the Description page of Project Settings.

#include "Insection2DApproach.h"
#include "PathWayPoint.h"
#include "Commons/PathfindingCommons.h"


void APathWayPoint::FindReachableWayPointsFromArray(UPARAM(ref) const TArray<APathWayPoint*>& WayPointPath)
{
    TArray<APathWayPoint*> tempWayPoints;

    UWorld* currentWorld = GetWorld();
    if (!currentWorld)
    {
        LogToScreen("ERROR - APathWayPoint::FindReachableWayPointsFromArray - Failed to get UWorld!");
        return;
    }
    
    // Setup line cast parameters
    FCollisionQueryParams traceParams = FCollisionQueryParams(FName(TEXT("PathfindingTrace")), true);
    traceParams.bTraceComplex = true;
    traceParams.bTraceAsyncScene = false;
    traceParams.bReturnPhysicalMaterial = false;
    FVector myLocation = GetActorLocation();
    FVector targetLocation;

    for (APathWayPoint* waypoint : WayPointPath)
    {
        if (waypoint == this)
            continue;

        targetLocation = waypoint->GetActorLocation();

        //Re-initialize hit info
        FHitResult TraceHit(ForceInit);
        currentWorld->LineTraceSingleByChannel(TraceHit, myLocation, targetLocation, TRACE_CHANNEL_ENEMIS, traceParams);
        if (!TraceHit.bBlockingHit)
        {
            // If nothing blocked LineCast then this path is valid
            tempWayPoints.Add(waypoint);
        }
    }

    // Now that we have reachable waypoints, we need to write them to appropiate field while making sure they are sorted by distance (closer ones first)
    APathWayPoint* closestWayPoint;
    float closestDist;
    float distance = 0.0f;
    ReachableWaypoints.Empty();
    while (tempWayPoints.Num() > 0) 
    {
        closestDist = 10000000.0f;
        for (APathWayPoint* waypoint : tempWayPoints)
        {
            distance = FVector::Dist(myLocation, waypoint->GetActorLocation());
            if (distance < closestDist)
            {
                closestWayPoint = waypoint;
                closestDist = distance;
            }
        }

        ReachableWaypoints.Add(closestWayPoint);
        tempWayPoints.Remove(closestWayPoint);
    }

    tempWayPoints.Empty();
}