// Conway's Game Of Life in Unreal
// Ilana Franklin, 2022

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "QuadTreeNode.h"
#include "BoardUtilities.h"

#include "GameBoard.generated.h"

/**
 * A square board used to simulate the Game of Life.
 */
UCLASS(BlueprintType, meta=(BlueprintSpawnableComponent))
class CONWAYSGAMEOFLIFE_API UGameBoard : public UObject
{
	GENERATED_BODY()

public:
	// Returns a UGameBoard with size BoardDimensionxBoardDimension.
	UFUNCTION(BlueprintCallable)
	static UGameBoard* InitializeBoardWithDimension(int BoardDimension);

	// Returns a UGameBoard with size kMaxSizeBoardxkMaxSizeBoard.
	UFUNCTION(BlueprintCallable)
	static UGameBoard* InitializeMaxSizeBoard();

private:
	// Helper used to construct an empty board with size BoardDimension.
	static UGameBoard* InitializeBoardHelper(uint64 BoardDimension);

public:
	// Sets the cell at FBoardCoordinate to alive. Should only be run before we've started simulating.
	UFUNCTION(BlueprintCallable)
	void SetCellToAlive(const FBoardCoordinate Coordinate);

	// Returns a string representing the state of the entire board.
	UFUNCTION(BlueprintCallable)
	FString GetBoardString() const;

	// Updates the board by one generation.
	UFUNCTION(BlueprintCallable)
	void SimulateNextGeneration();

	// Returns a string representing a portion of the board indicated by DesiredDimension and Coordinate. For Debug purposes.
	FString GetBoardStringForBlockOfDimensionContainingCoordinate(uint64 DesiredDimension, const FBoardCoordinate Coordinate) const;

	// Populates an array of FBoardCoordinates with the location of every live cell in a portion of the board indicated by the desired dimension and coordinate to find.
	void GetLocalLiveCellCoordinatesFromFoundBlock(uint64 DesiredDimensionOfBlock, const FBoardCoordinate CoordinateToFind, TArray<FBoardCoordinate>& ResultsOut) const;

	TSharedPtr<const QuadTreeNode> GetBlockOfDimensionContainingCoordinate(uint64 DesiredDimensionOfBlock, uint64 X, uint64 Y) const;
	
private:
	// The dimensions of the board on one side. Must be a power of two. Boards are always square.
	uint64 mBoardDimension;

	// The level of the root node in the tree.
	uint8 mMaxLevelInTree;

	// Root node of the quadtree representing our current board.
	TSharedPtr<const QuadTreeNode> mRootNode;

	// Given a quadrant, returns the quadrant that is above or below it.
	ChildNode GetOpposingVerticalQuadrant(ChildNode Child) const;
	
	// Given a quadrant, returns the quadrant that is to the left or right of it.
	ChildNode GetOpposingHorizontalQuadrant(ChildNode Child) const;

	// Given a quadrant, returns the quadrant that is diagonal, from it.
	ChildNode GetOpposingDiagonalQuadrant(ChildNode Child) const;

	// Constructs a new board with the provided quadrant in the center. Will have dimension (mBoardDimension / 2).
	TSharedPtr<const QuadTreeNode> ConstructBoardWithCenteredQuadrant(ChildNode QuadrantToCenter) const;
};

