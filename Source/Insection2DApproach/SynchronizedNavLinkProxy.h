// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AI/Navigation/NavLinkProxy.h"
#include "SynchronizedNavLinkProxy.generated.h"

/**
 * 
 */
UCLASS()
class INSECTION2DAPPROACH_API ASynchronizedNavLinkProxy : public ANavLinkProxy
{
	GENERATED_BODY()
    UFUNCTION(BlueprintCallable, Category = "Custom")
    void SynchronizeLinks();

    virtual void PostLoad() override;
};
