// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "QuadTreeNode.h"

#include "GameBoard.generated.h"


/**
 * 
 */
UCLASS(BlueprintType, meta=(BlueprintSpawnableComponent))
class CONWAYSGAMEOFLIFE_API UGameBoard : public UObject
{
	GENERATED_BODY()

public:
	// TODO add error handling for trying to construct a new board when one already exists?
	UFUNCTION(BlueprintCallable)
	void ConstructBoard(int BoardDimension);

	UFUNCTION(BlueprintCallable)
	void ConstructMaxSizeBoard();

	UFUNCTION(BlueprintCallable)
	void SetBit(const int64 X, const int64 Y);

	UFUNCTION(BlueprintCallable)
	FString GetBoardString() const;

	UFUNCTION(BlueprintCallable)
	void SimulateNextGeneration();

	UFUNCTION(BlueprintCallable)
	FString GetBoardStringForBlockOfDimensionContainingCoordinate(int DesiredDimension, int64 X, int64 Y) const;
	
private:
	// The dimensions of the board on one side. Must be a power of two. Boards are always square.
	uint64 mBoardDimension;

	// The level of the root node in the tree.
	uint8 mMaxLevelInTree;

	// Root node of the quadtree representing our current board.
	TSharedPtr<const QuadTreeNode> mRootNode;

	// Constructs a new board with the provided quadrant in the center. Will have dimension (mBoardDimension / 2).
	TSharedPtr<const QuadTreeNode> ConstructBoardWithCenteredQuadrant(ChildNode QuadrantToCenter) const;

	// Helper used to construct an empty board with size BoardDimension.
	void ConstructBoardHelper(uint64 BoardDimension);
};

