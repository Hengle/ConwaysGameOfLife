// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BoardUtilities.generated.h"

/**
 *
 */
USTRUCT(BlueprintType)
struct FBoardCoordinate
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	int64 mX = 0;

	UPROPERTY(BlueprintReadWrite)
	int64 mY = 0;

	void SetXAndY(int64 X, int64 Y)
	{
		mX = X;
		mY = Y;
	}
};

/**
 * 
 */
UCLASS()
class CONWAYSGAMEOFLIFE_API UBoardUtilities : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
};
