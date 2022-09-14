// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "QuadTreeNode.h"

#include "GameBoard.generated.h"

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
UCLASS(BlueprintType, meta=(BlueprintSpawnableComponent))
class CONWAYSGAMEOFLIFE_API UGameBoard : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	static UGameBoard* InitializeBoardWithDimension(int BoardDimension);

	UFUNCTION(BlueprintCallable)
	static UGameBoard* InitializeMaxSizeBoard();

private:
	// Helper used to construct an empty board with size BoardDimension.
	static UGameBoard* InitializeBoardHelper(uint64 BoardDimension);

public:

	UFUNCTION(BlueprintCallable)
	void SetBit(const FBoardCoordinate Coordinate);

	UFUNCTION(BlueprintCallable)
	FString GetBoardString() const;

	UFUNCTION(BlueprintCallable)
	void SimulateNextGeneration();

	UFUNCTION(BlueprintCallable)
	FString GetBoardStringForBlockOfDimensionContainingCoordinate(int DesiredDimension, const FBoardCoordinate Coordinate) const;

	UFUNCTION(BlueprintCallable)
	void GetLocalLiveCellCoordinatesFromFoundBlock(int DesiredDimensionOfBlock, const FBoardCoordinate CoordinateToFind, TArray<FBoardCoordinate>& ResultsOut) const;
	
private:
	// The dimensions of the board on one side. Must be a power of two. Boards are always square.
	uint64 mBoardDimension;

	// The level of the root node in the tree.
	uint8 mMaxLevelInTree;

	// Root node of the quadtree representing our current board.
	TSharedPtr<const QuadTreeNode> mRootNode;

	ChildNode GetOpposingVerticalQuadrant(ChildNode Child) const;

	ChildNode GetOpposingHorizontalQuadrant(ChildNode Child) const;

	ChildNode GetOpposingDiagonalQuadrant(ChildNode Child) const;


	// Constructs a new board with the provided quadrant in the center. Will have dimension (mBoardDimension / 2).
	TSharedPtr<const QuadTreeNode> ConstructBoardWithCenteredQuadrant(ChildNode QuadrantToCenter) const;


};

