// Fill out your copyright notice in the Description page of Project Settings.


#include "BoardVisualizerSection.h"

#include "GameBoard.h"

// Sets default values
ABoardVisualizerSection::ABoardVisualizerSection()
{
 	// This Actor should not tick, it should be told when to update via the Controller.
	PrimaryActorTick.bCanEverTick = false;
}


void ABoardVisualizerSection::InitializeWithSectionDimension_Implementation(int SectionDimension)
{
	mSectionDimension = SectionDimension;
}

void ABoardVisualizerSection::UpdateRepresentation(const UGameBoard* GameBoard)
{
	if (GameBoard != nullptr)
	{
		TSharedPtr<const QuadTreeNode> BlockToRepresent = GameBoard->GetBlockOfDimensionContainingCoordinate(mSectionDimension, mXCoordinateToRepresent, mYCoordinateToRepresent);

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
