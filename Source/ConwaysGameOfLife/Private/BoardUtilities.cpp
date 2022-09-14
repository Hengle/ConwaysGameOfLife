// Conway's Game Of Life in Unreal
// Ilana Franklin, 2022


#include "BoardUtilities.h"

#include "Misc/DefaultValueHelper.h"

void UBoardUtilities::ParseStringIntoCoordinates(FString SourceString, TArray<FBoardCoordinate>& ResultsOut)
{
	ResultsOut.Empty();

	// Remove parens from our coordinates.
	FString NoParentheses = SourceString.Replace(*FString("("), *FString(" "));
	NoParentheses = NoParentheses.Replace(*FString(")"), *FString(" "));

	// Separate into strings for each coordinate.
	TArray<FString> CoordinateStrings;
	NoParentheses.ParseIntoArray(CoordinateStrings, *FString("\n"));

	TArray<FString> TempCoordinateHolder;
	const FString Delimiter = FString(",");
	for (const FString CoordinateString : CoordinateStrings)
	{
		TempCoordinateHolder.Empty();

		// Get our two coordinates as strings.
		CoordinateString.ParseIntoArray(TempCoordinateHolder, *Delimiter);

		// Try to convert these coordinates to int64s, then add them to our results.
		int64 NewX, NewY;
		FBoardCoordinate NewCoordinate;
		if (FDefaultValueHelper::ParseInt64(TempCoordinateHolder[0], NewX) && FDefaultValueHelper::ParseInt64(TempCoordinateHolder[1], NewY))
		{
			NewCoordinate.SetXAndY(NewX, NewY);
			ResultsOut.AddUnique(NewCoordinate);
		}
	}
}

int64 UBoardUtilities::ParseStringToInt64(FString SourceString)
{
	int64 Result = 0;
	FDefaultValueHelper::ParseInt64(SourceString, Result);
	return Result;
}