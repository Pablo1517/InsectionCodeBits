// Fill out your copyright notice in the Description page of Project Settings.

#include "Insection2DApproach.h"

IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, Insection2DApproach, "Insection2DApproach" );

//General Log
DEFINE_LOG_CATEGORY(General);

//Logging during game startup
DEFINE_LOG_CATEGORY(InsectionInit);

//Logging for your AI system
DEFINE_LOG_CATEGORY(AISystem);

//Logging for Critical Errors that must always be addressed
DEFINE_LOG_CATEGORY(CriticalErrors);


void LogToScreen(FString message, float time, FColor color)
{
    if (GEngine) 
        GEngine->AddOnScreenDebugMessage(-1, time, FColor::White, message);
}