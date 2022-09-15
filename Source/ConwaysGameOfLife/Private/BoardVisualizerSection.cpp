// Conway's Game Of Life in Unreal
// Ilana Franklin, 2022

#include "BoardVisualizerSection.h"

#include "GameBoard.h"

// Sets default values
ABoardVisualizerSection::ABoardVisualizerSection()
{
 	// This Actor should not tick, it should be told when to update via the Controller.
	PrimaryActorTick.bCanEverTick = false;
}

void ABoardVisualizerSection::SetCoordinateToRepresent(FBoardCoordinate Coordinate)
{
	mXCoordinateToRepresent = Coordinate.mX;
	mYCoordinateToRepresent = Coordinate.mY;
}

void ABoardVisualizerSection::UpdateRepresentation(const UGameBoard* GameBoard)
{
	if (GameBoard != nullptr)
	{
		TSharedPtr<const QuadTreeNode> BlockToRepresent = GameBoard->GetBlockOfDimensionContainingCoordinate(mSectionDimension, mXCoordinateToRepresent, mYCoordinateToRepresent);

		// Hide dead cells and reveal live cells.
		for (auto& Cell : mCoordinateToCellActorMap)
		{
			if (BlockToRepresent->GetIsCellAlive(Cell.Key.mX, Cell.Key.mY))
			{
				Cell.Value->SetActorHiddenInGame(false);
			}
			else
			{
				Cell.Value->SetActorHiddenInGame(true);
			}
		}
	}
}

void ABoardVisualizerSection::AddCellToMap(int64 LocalXCoordinate, int64 LocalYCoordinate, AActor* Cell)
{
	FBoardCoordinate Coordinate;
	Coordinate.SetXAndY(LocalXCoordinate, LocalYCoordinate);
	mCoordinateToCellActorMap.Add(Coordinate, Cell);
}
