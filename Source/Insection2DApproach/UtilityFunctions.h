// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "UtilityFunctions.generated.h"


#pragma region ENUMS

UENUM(BlueprintType)        //"BlueprintType" is essential specifier
namespace EComparisonResult
{
    //256 entries max
    enum Type
    {
        VE_Equal     UMETA(DisplayName = "Equal"),
        VE_Greater   UMETA(DisplayName = "Greater"),
        VE_Lower     UMETA(DisplayName = "Lower"),
        VE_Unknown     UMETA(DisplayName = "Unknown"),
    };
}

#pragma endregion ENUMS


#pragma region STRUCTS

USTRUCT(BlueprintType)
struct FComparisonResultStruct : public FTableRowBase
{
    GENERATED_USTRUCT_BODY()

        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Comparison Result")
        uint8 ComparisonResultIndex;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Comparison Result")
        FText ComparisonResult_Name;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Comparison Result")
        TEnumAsByte<EComparisonResult::Type> ComparisonResult_State;
};

#pragma endregion STRUCTS

/**
 * 
 */
UCLASS()
class INSECTION2DAPPROACH_API UUtilityFunctions : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
	
public:
    UFUNCTION(BlueprintCallable, Category = "Game|Utility")
        static bool BPSetMousePosition(const APlayerController* playerController, const float LocationX, const float LocationY);
	
    UFUNCTION(BlueprintCallable, Category = "Game|Utility")
        static FVector2D BPGetAimPosition(const APlayerController* playerController, const FVector2D lastAim, const float minRadius, const float maxRadius, const bool invert);

    //** Compares two Vectors and returns Enum "Greater" if PointA.X > PointB.X and PointA.Y > PointB.Y and so on. Will return "Unknown" if Vectors are entirely different, not great, not equal and not lower */
    UFUNCTION(BlueprintCallable, Category = "Game|Utility")
        static EComparisonResult::Type CompareVectors(const FVector& PointA, const FVector& PointB);
};