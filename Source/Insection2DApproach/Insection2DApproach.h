// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine.h"

//General Log
DECLARE_LOG_CATEGORY_EXTERN(General, Log, All);

//Logging during game startup
DECLARE_LOG_CATEGORY_EXTERN(InsectionInit, Log, All);

//Logging for your AI system
DECLARE_LOG_CATEGORY_EXTERN(AISystem, Log, All);

//Logging for Critical Errors that must always be addressed
DECLARE_LOG_CATEGORY_EXTERN(CriticalErrors, Log, All);

void LogToScreen(FString message, float time = 1.5f, FColor color = FColor::White);