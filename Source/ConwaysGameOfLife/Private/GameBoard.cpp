// Fill out your copyright notice in the Description page of Project Settings.


#include "GameBoard.h"

void UGameBoard::ConstructBoard(int BoardDimension)
{
	// TODO Also check for some board minsize!
	if (BoardDimension < 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Attempting to call ConstructBoard with a BoardDimension that is too small. Board Dimension must be at least TODO"));
		return;
	}

	ConstructBoardHelper(BoardDimension);
}

void UGameBoard::ConstructMaxSizeBoard()
{
	ConstructBoardHelper(UINT64_MAX);
}

void UGameBoard::ConstructBoardHelper(uint64 BoardDimension)
{
	if (mRootNode.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Calling ConstructBoard on a GameBoard that already contains data. This will erase the old GameBoard state."));
	}

	mBoardDimension = BoardDimension;
	mMaxLevelInTree = log2(mBoardDimension);

	mRootNode = QuadTreeNode::CreateEmptyNode(mMaxLevelInTree);
}

void UGameBoard::SetBit(const int64 X, const int64 Y)
{
	mRootNode = mRootNode->SetCellToAlive(X, Y);
}

ChildNode GetOpposingVerticalQuadrant(ChildNode Child)
{
	switch (Child)
	{
	case ChildNode::Northwest:
		return ChildNode::Southwest;
	case ChildNode::Northeast:
		return ChildNode::Southeast;
	case ChildNode::Southwest:
		return ChildNode::Northwest;
	case ChildNode::Southeast:
		return ChildNode::Northeast;
	}


	return ChildNode::Northwest;
	// TODO add an error case here.
}

// TODO make these part of the game board class!! Or part of a helper class.
ChildNode GetOpposingHorizontalQuadrant(ChildNode Child)
{
	switch (Child)
	{
	case ChildNode::Northwest:
		return ChildNode::Northeast;
	case ChildNode::Northeast:
		return ChildNode::Northwest;
	case ChildNode::Southwest:
		return ChildNode::Southeast;
	case ChildNode::Southeast:
		return ChildNode::Southwest;
	}	
	
	return ChildNode::Northwest;
	// TODO add an error case here
}

ChildNode GetOpposingDiagonalQuadrant(ChildNode Child)
{
	switch (Child)
	{
	case ChildNode::Northwest:
		return ChildNode::Southeast;
	case ChildNode::Northeast:
		return ChildNode::Southwest;
	case ChildNode::Southwest:
		return ChildNode::Northeast;
	case ChildNode::Southeast:
		return ChildNode::Northwest;
	}

	return ChildNode::Northwest;
	// TODO add an error case here
}

TSharedPtr<const QuadTreeNode> UGameBoard::ConstructBoardWithCenteredQuadrant(ChildNode QuadrantToCenter) const
{
	TSharedPtr<const QuadTreeNode> MainQuadrant = mRootNode->GetChild(QuadrantToCenter);
	TSharedPtr<const QuadTreeNode> OpposingHorizontal = mRootNode->GetChild(GetOpposingHorizontalQuadrant(QuadrantToCenter));
	TSharedPtr<const QuadTreeNode> OpposingVertical = mRootNode->GetChild(GetOpposingVerticalQuadrant(QuadrantToCenter));
	TSharedPtr<const QuadTreeNode> OpposingDiagonal = mRootNode->GetChild(GetOpposingDiagonalQuadrant(QuadrantToCenter));

	const TSharedPtr<const QuadTreeNode> NewNorthwest = QuadTreeNode::CreateNodeWithSubnodes(mMaxLevelInTree - 1,
		OpposingDiagonal->GetChild(ChildNode::Southeast),
		OpposingVertical->GetChild(ChildNode::Southwest),
		OpposingHorizontal->GetChild(ChildNode::Northeast),
		MainQuadrant->GetChild(ChildNode::Northwest));

	const TSharedPtr<const QuadTreeNode> NewNortheast = QuadTreeNode::CreateNodeWithSubnodes(mMaxLevelInTree - 1,
		OpposingVertical->GetChild(ChildNode::Southeast),
		OpposingDiagonal->GetChild(ChildNode::Southwest),
		MainQuadrant->GetChild(ChildNode::Northeast),
		OpposingHorizontal->GetChild(ChildNode::Northwest));

	const TSharedPtr<const QuadTreeNode> NewSouthwest = QuadTreeNode::CreateNodeWithSubnodes(mMaxLevelInTree - 1,
		OpposingHorizontal->GetChild(ChildNode::Southeast),
		MainQuadrant->GetChild(ChildNode::Southwest),
		OpposingDiagonal->GetChild(ChildNode::Northeast),
		OpposingVertical->GetChild(ChildNode::Northwest));

	const TSharedPtr<const QuadTreeNode> NewSoutheast = QuadTreeNode::CreateNodeWithSubnodes(mMaxLevelInTree - 1,
		MainQuadrant->GetChild(ChildNode::Southeast),
		OpposingHorizontal->GetChild(ChildNode::Southwest),
		OpposingVertical->GetChild(ChildNode::Northeast),
		OpposingDiagonal->GetChild(ChildNode::Northwest));

	return QuadTreeNode::CreateNodeWithSubnodes(mMaxLevelInTree, NewNorthwest, NewNortheast, NewSouthwest, NewSoutheast);
}

void UGameBoard::SimulateNextGeneration()
{
	TSharedPtr<const QuadTreeNode> NorthwestQuadrant, NortheastQuadrant, SouthwestQuadrant, SoutheastQuadrant;

	// TODO Need at least 3 levels! 
	// const int startingLevel = log2(sBoardSize);

	const TSharedPtr<const QuadTreeNode> SolvedNorthwest = ConstructBoardWithCenteredQuadrant(ChildNode::Northwest)->GetNextGeneration();
	const TSharedPtr<const QuadTreeNode> SolvedNortheast = ConstructBoardWithCenteredQuadrant(ChildNode::Northeast)->GetNextGeneration();
	const TSharedPtr<const QuadTreeNode> SolvedSouthwest = ConstructBoardWithCenteredQuadrant(ChildNode::Southwest)->GetNextGeneration();
	const TSharedPtr<const QuadTreeNode> SolvedSoutheast = ConstructBoardWithCenteredQuadrant(ChildNode::Southeast)->GetNextGeneration();

	mRootNode = QuadTreeNode::CreateNodeWithSubnodes(mMaxLevelInTree, SolvedNorthwest, SolvedNortheast, SolvedSouthwest, SolvedSoutheast);
}

FString UGameBoard::GetBoardString() const
{
	return mRootNode->GetNodeString();
}