// Fill out your copyright notice in the Description page of Project Settings.

#include "Insection2DApproach.h"
#include "SynchronizedNavLinkProxy.h"
#include "AI/Navigation/NavLinkCustomComponent.h"

void ASynchronizedNavLinkProxy::SynchronizeLinks()
{
    auto smartLink = GetSmartLinkComp();

    if (!smartLink || PointLinks.Num() == 0)
        return;
    auto simpleLink = PointLinks[0];
   
    smartLink->SetLinkData(simpleLink.Left, simpleLink.Right, simpleLink.Direction);

}

void ASynchronizedNavLinkProxy::PostLoad()
{
    Super::PostLoad();
    SynchronizeLinks();
}
