// Fill out your copyright notice in the Description page of Project Settings.

#include "Insection2DApproach.h"
#include "UtilityFunctions.h"


/* static */bool UUtilityFunctions::BPSetMousePosition(const APlayerController* playerController, const float LocationX, const float LocationY)
{
    if (!playerController)
        return false;

    ULocalPlayer* localPlayer = CastChecked<ULocalPlayer>(playerController->Player);
    if (!localPlayer)
        return false;

    UGameViewportClient* viewportClient = localPlayer->ViewportClient;
    if (!viewportClient)
        return false;

    FViewport* vp = viewportClient->Viewport;
    if (!vp)
        return false;

    int intX = (int)LocationX;
    int intY = (int)LocationY;
    vp->SetMouse(intX, intY);
    return true;
}

/*static */FVector2D UUtilityFunctions::BPGetAimPosition(const APlayerController* playerController, const FVector2D lastAim, const float minRadius, const float maxRadius, const bool invert)
{
    //mode switches
    //bool invert = false; //Avaliable as parameter // if set Aim position will be returned as reflection with player position used as origin.

    //static data
    static bool OnBorder = false; //set to true if during last check mouse was on border created by maxRadius
    static bool MoveClockwise = false;
    static FVector2D MouseDir;

    if (playerController == nullptr)
        return FVector2D();

    int W, H;
    playerController->GetViewportSize(W, H);
    FVector2D screenCenter(W / 2, H / 2);
    //
    FVector2D playerPos, mousePos, correctedMousePos;
    playerController->GetMousePosition(mousePos.X, mousePos.Y);

    playerController->ProjectWorldLocationToScreen(playerController->GetPawn()->GetActorLocation(), playerPos);

    auto lastMousePos = invert ? 2 * screenCenter - lastAim : lastAim;

    auto delta = mousePos - screenCenter;
    float len;
    FVector2D dir;
    delta.ToDirectionAndLength(dir, len);
    FVector2D MouseSimplifiedDir(dir.X >= 0 ? 1 : -1, dir.Y >= 0 ? 1 : -1);

    if (len > maxRadius)
    {

        correctedMousePos = screenCenter + dir*maxRadius;

//         float dlen;
//         FVector2D ddir;
//         (mousePos - lastMousePos).ToDirectionAndLength(ddir, dlen);
//         auto dangle = dlen / maxRadius;
//         auto deg = dangle * 180 / 3.1415;
//         auto currentAngle = atan2f(-(lastMousePos.Y - screenCenter.Y), lastMousePos.X - screenCenter.X);
//         auto deg2 = currentAngle * 180 / 3.1415;
//         auto p1 = screenCenter + FVector2D(cos(currentAngle + dangle), -sin(currentAngle + dangle))*maxRadius;
//         auto p2 = screenCenter + FVector2D(cos(currentAngle - dangle), -sin(currentAngle - dangle))*maxRadius;
// 
//         if (OnBorder && MouseSimplifiedDir == MouseDir)
//         {
//             if (MoveClockwise)
//             {
//                 correctedMousePos = p2;
//             }
//             else
//             {
//                 correctedMousePos = p1;
//             }
// 
//             auto localDir = correctedMousePos - lastMousePos;
//             localDir = FVector2D(localDir.X >= 0 ? 1 : -1, localDir.Y >= 0 ? 1 : -1);
//             if (FVector2D::CrossProduct(MouseDir, localDir) == 0)
//                 correctedMousePos = lastMousePos;
//         }
//         else
//         {
//             if ((p1 - mousePos).Size() + (p1 - lastMousePos).Size() < (p2 - mousePos).Size() + (p2 - lastMousePos).Size())
//             {
//                 correctedMousePos = p1;
//                 MoveClockwise = false;
//             }
//             else
//             {
//                 correctedMousePos = p2;
//                 MoveClockwise = true;
//             }
//             OnBorder = true;
//         }
    }
    else if (len < minRadius)
    {   
        float dlen;
        FVector2D ddir;
        (mousePos - lastMousePos).ToDirectionAndLength(ddir, dlen);
        auto dangle = dlen / minRadius;
        auto deg = dangle * 180 / 3.1415;
        auto currentAngle = atan2f(-(lastMousePos.Y - screenCenter.Y), lastMousePos.X - screenCenter.X);
        auto deg2 = currentAngle * 180 / 3.1415;
        auto p1 = screenCenter + FVector2D(cos(currentAngle + dangle), -sin(currentAngle + dangle))*minRadius;
        auto p2 = screenCenter + FVector2D(cos(currentAngle - dangle), -sin(currentAngle - dangle))*minRadius;

        if (OnBorder && MouseSimplifiedDir == MouseDir)
        {
            if (MoveClockwise)
            {
                correctedMousePos = p2;
            }
            else
            {
                correctedMousePos = p1;
            }

            auto localDir = correctedMousePos - lastMousePos;
            localDir = FVector2D(localDir.X >= 0 ? 1 : -1, localDir.Y >= 0 ? 1 : -1);
            if (FVector2D::CrossProduct(MouseDir, localDir) == 0)
                correctedMousePos = lastMousePos;
        }
        else
        {
            if ((p1 - mousePos).Size() + (p1 - lastMousePos).Size() < (p2 - mousePos).Size() + (p2 - lastMousePos).Size())
            {
                correctedMousePos = p1;
                MoveClockwise = false;
            }
            else
            {
                correctedMousePos = p2;
                MoveClockwise = true;
            }
            OnBorder = true;
        }


//         OnBorder = false;
//         float dlen;
//         FVector2D ddir;
//         (mousePos - lastMousePos).ToDirectionAndLength(ddir,dlen);
//         auto dangle = dlen / minRadius;
//         auto deg = dangle * 180 / 3.1415;
//         auto currentAngle = atan2f(-(lastMousePos.Y-screenCenter.Y), lastMousePos.X-screenCenter.X);
//         auto deg2 = currentAngle * 180 / 3.1415;
//         auto p1 = screenCenter + FVector2D(cos(currentAngle+dangle),-sin(currentAngle + dangle))*minRadius;
//         auto p2 = screenCenter + FVector2D(cos(currentAngle - dangle),-sin(currentAngle - dangle))*minRadius;     
// 
// 
//         if ((p1 - mousePos).Size()+(p1- lastMousePos).Size() < (p2 - mousePos).Size()+(p2- lastMousePos).Size())
//         {
//             correctedMousePos = p1;
//         }
//         else
//         {
//             correctedMousePos = p2;
//         }
//         if ((correctedMousePos - lastMousePos).Size() < 2.0f)
//             correctedMousePos = lastMousePos;
    }
    else
    {
        OnBorder = false;
        correctedMousePos = mousePos;
    }
    MouseDir = MouseSimplifiedDir;
    BPSetMousePosition(playerController, correctedMousePos.X, correctedMousePos.Y);

    auto aimPos = invert ? 2 * screenCenter - correctedMousePos : correctedMousePos;

    //move from screenCenter coordinates to 'player screen' coordinates
    aimPos = aimPos - screenCenter + playerPos;
    return aimPos;
}

EComparisonResult::Type UUtilityFunctions::CompareVectors(const FVector& PointA, const FVector& PointB)
{
    if (PointA.X == PointB.X 
    && PointA.Y == PointB.Y
    && PointA.Z == PointB.Z)
    {
        return EComparisonResult::VE_Equal;
    }
    if (PointA.X > PointB.X
    && PointA.Y > PointB.Y
    && PointA.Z > PointB.Z)
    {
        return EComparisonResult::VE_Greater;
    }
    if (PointA.X < PointB.X
    && PointA.Y < PointB.Y
    && PointA.Z < PointB.Z)
    {
        return EComparisonResult::VE_Lower;
    }
       
    return EComparisonResult::VE_Unknown;
}