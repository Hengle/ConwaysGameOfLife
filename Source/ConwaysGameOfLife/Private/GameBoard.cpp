// Fill out your copyright notice in the Description page of Project Settings.


#include "GameBoard.h"

void UGameBoard::CreateTree()
{
	// TEMP
	const int startingLevel = log2(sBoardSize);

	mRootNode = QuadTreeNode::CreateEmptyNode(startingLevel);

	UE_LOG(LogTemp, Warning, TEXT("Done creating empty quadtree."));
}

void UGameBoard::SetBit(const int64 X, const int64 Y)
{
	mRootNode = mRootNode->SetCellToAlive(X, Y);
}

void UGameBoard::SimulateNextGeneration()
{
	TSharedPtr<const QuadTreeNode> NorthwestQuadrant, NortheastQuadrant, SouthwestQuadrant, SoutheastQuadrant;

	// Need at least 3 levels! 

	// Find the solution for the 

	mRootNode = mRootNode->GetNextGeneration();
}

FString UGameBoard::GetBoardString() const
{
	if (!mRootNode.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("mRootNode on GameBoard is invalid!"));
		return "ERROR";
	}

	FString Result = "\n";

	for (int YIter = 0; YIter < sBoardSize; ++YIter)
	{
		for (int XIter = 0; XIter < sBoardSize; ++XIter)
		{
			if (mRootNode->GetIsCellAlive(XIter, YIter))
			{
				Result += "A ";
			}
			else
			{
				Result += ". ";
			}
		}

		Result += "\n";

	}

	return Result;
}