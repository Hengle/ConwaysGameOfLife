// Fill out your copyright notice in the Description page of Project Settings.


#include "GameBoard.h"

void UGameBoard::CreateTree()
{
	// TEMP
	const int startingLevel = 3;

	mRootNode = MakeShareable<QuadTreeNode>(new QuadTreeNode(startingLevel));
}

void UGameBoard::SetBit(const int64 X, const int64 Y)
{
	mRootNode = mRootNode->SetBit(X, Y);
}

void UGameBoard::SimulateNextGeneration()
{
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
			if (mRootNode->GetBit(XIter, YIter))
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