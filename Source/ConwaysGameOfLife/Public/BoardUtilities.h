// Conway's Game Of Life in Unreal
// Ilana Franklin, 2022
#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BoardUtilities.generated.h"

/**
 * Structure to hold one coordinate on the Game Of Life board.
 */
USTRUCT(BlueprintType)
struct FBoardCoordinate
{
	GENERATED_BODY()

public:
	// The X value of the coordinate.
	UPROPERTY(BlueprintReadWrite)
	int64 mX = 0;

	// The Y value of the coordinate.
	UPROPERTY(BlueprintReadWrite)
	int64 mY = 0;

	// Sets the values of mX and mY at the same time.
	void SetXAndY(int64 X, int64 Y)
	{
		mX = X;
		mY = Y;
	}

	bool operator==(const FBoardCoordinate& Other) const
	{
		return (mX == Other.mX) && (mY == Other.mY);
	}
};

/**
 * Various helper functions for Game of Life.
 */
UCLASS()
class CONWAYSGAMEOFLIFE_API UBoardUtilities : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// Parses a string of coordinate separated by newlines into an array of FBoardCoordinates.
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static void ParseStringIntoCoordinates(FString SourceString, TArray<FBoardCoordinate>& ResultsOut);
	
	// Converts a string to an int64.
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static int64 ParseStringToInt64(FString SourceString);
	
};
