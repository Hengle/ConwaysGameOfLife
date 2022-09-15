// Conway's Game Of Life in Unreal
// Ilana Franklin, 2022

#include "GameBoard.h"

UGameBoard* UGameBoard::InitializeBoardWithDimension(int BoardDimension)
{
	constexpr int MinBoardSize = 8;

	if (BoardDimension < MinBoardSize)
	{
		UE_LOG(LogTemp, Error, TEXT("Attempting to call InitializeBoardWithDimension with a BoardDimension that is too small. Board Dimension must be at least TODO"));
		return nullptr;
	}

	return InitializeBoardHelper(16);
}

UGameBoard* UGameBoard::InitializeMaxSizeBoard()
{
	return InitializeBoardHelper(kMaxSizeBoard);
}

UGameBoard* UGameBoard::InitializeBoardHelper(uint64 BoardDimension)
{
	if (UGameBoard* ResultPointer = NewObject<UGameBoard>())
	{
		ResultPointer->mBoardDimension = BoardDimension;
		ResultPointer->mMaxLevelInTree = log2(BoardDimension);

		ResultPointer->mRootNode = QuadTreeNode::CreateEmptyNode(ResultPointer->mMaxLevelInTree);

		return ResultPointer;
	}

	return nullptr;
}

void UGameBoard::SetCellToAlive(const FBoardCoordinate Coordinate)
{
	mRootNode = mRootNode->SetCellToAlive(Coordinate.mX, Coordinate.mY);
}

ChildNode UGameBoard::GetOpposingVerticalQuadrant(ChildNode Child) const
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
	default:
		UE_LOG(LogTemp, Warning, TEXT("Attempted to get opposing quadrant data for some unknown ChildNode. Did we forget to add a case?"));
		return ChildNode::Northwest;
	}
}

// TODO make these part of the game board class!! Or part of a helper class.
ChildNode UGameBoard::GetOpposingHorizontalQuadrant(ChildNode Child) const
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
	default:
		UE_LOG(LogTemp, Warning, TEXT("Attempted to get opposing quadrant data for some unknown ChildNode. Did we forget to add a case?"));
		return ChildNode::Northwest;
	}	
}

ChildNode UGameBoard::GetOpposingDiagonalQuadrant(ChildNode Child) const
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
	default:
		UE_LOG(LogTemp, Warning, TEXT("Attempted to get opposing quadrant data for some unknown ChildNode. Did we forget to add a case?"));
		return ChildNode::Northwest;
	}
}

TSharedPtr<const QuadTreeNode> UGameBoard::ConstructBoardWithCenteredQuadrant(ChildNode QuadrantToCenter) const
{
	TSharedPtr<const QuadTreeNode> MainQuadrant = mRootNode->GetChild(QuadrantToCenter);
	TSharedPtr<const QuadTreeNode> OpposingHorizontal = mRootNode->GetChild(GetOpposingHorizontalQuadrant(QuadrantToCenter));
	TSharedPtr<const QuadTreeNode> OpposingVertical = mRootNode->GetChild(GetOpposingVerticalQuadrant(QuadrantToCenter));
	TSharedPtr<const QuadTreeNode> OpposingDiagonal = mRootNode->GetChild(GetOpposingDiagonalQuadrant(QuadrantToCenter));

	TSharedPtr<const QuadTreeNode> EmptyTree = QuadTreeNode::CreateEmptyNode(mMaxLevelInTree - 2);
	/*
	(-5, 5)
	(-4, 4)
	(-3, 4)
	(-3, 5)
	(-3, 6)
		*/
	
	const TSharedPtr<const QuadTreeNode> NewNorthwest = QuadTreeNode::CreateNodeWithSubnodes(mMaxLevelInTree - 1,
		OpposingDiagonal->Southeast(),
		OpposingVertical->Southwest(),
		OpposingHorizontal->Northeast(),
		MainQuadrant->Northwest());

	const TSharedPtr<const QuadTreeNode> NewNortheast = QuadTreeNode::CreateNodeWithSubnodes(mMaxLevelInTree - 1,
		OpposingVertical->Southeast(),
		OpposingDiagonal->Southwest(),
		MainQuadrant->Northeast(),
		OpposingHorizontal->Northwest());

	const TSharedPtr<const QuadTreeNode> NewSouthwest = QuadTreeNode::CreateNodeWithSubnodes(mMaxLevelInTree - 1,
		OpposingHorizontal->Southeast(),
		MainQuadrant->Southwest(),
		OpposingDiagonal->Northeast(),
		OpposingVertical->Northwest());

	const TSharedPtr<const QuadTreeNode> NewSoutheast = QuadTreeNode::CreateNodeWithSubnodes(mMaxLevelInTree - 1,
		MainQuadrant->Southeast(),
		OpposingHorizontal->Southwest(),
		OpposingVertical->Northeast(),
		OpposingDiagonal->Northwest());
	/*

	const TSharedPtr<const QuadTreeNode> NewNorthwest = QuadTreeNode::CreateNodeWithSubnodes(mMaxLevelInTree - 1,
		EmptyTree,
		EmptyTree,
		EmptyTree,
		MainQuadrant->Northwest());

	const TSharedPtr<const QuadTreeNode> NewNortheast = QuadTreeNode::CreateNodeWithSubnodes(mMaxLevelInTree - 1,
		EmptyTree,
		EmptyTree,
		MainQuadrant->Northeast(),
		OpposingHorizontal->Northwest());

	const TSharedPtr<const QuadTreeNode> NewSouthwest = QuadTreeNode::CreateNodeWithSubnodes(mMaxLevelInTree - 1,
		EmptyTree,
		MainQuadrant->Southwest(),
		EmptyTree,
		OpposingVertical->Northwest());

	const TSharedPtr<const QuadTreeNode> NewSoutheast = QuadTreeNode::CreateNodeWithSubnodes(mMaxLevelInTree - 1,
		MainQuadrant->Southeast(),
		OpposingHorizontal->Southwest(),
		OpposingVertical->Northeast(),
		EmptyTree);
	*/
	return QuadTreeNode::CreateNodeWithSubnodes(mMaxLevelInTree, NewNorthwest, NewNortheast, NewSouthwest, NewSoutheast);
}

void UGameBoard::SimulateNextGeneration()
{
	TSharedPtr<const QuadTreeNode> NorthwestQuadrant, NortheastQuadrant, SouthwestQuadrant, SoutheastQuadrant;

	/*
	/**
	* Create four new trees. Each one will have one quadrant of our board in the center.
	* In parallel, we go through and calculate the next generation on each of these new trees.
	* This will give us the solved version of that quadrant. 
	* To get our final solved board, we can recombine our quadrants in the correct order.

	TStaticArray<TSharedPtr<const QuadTreeNode>, 4> SolvedChildQuadrants;

	ParallelFor(ChildNode::kCount, [&](int32 QuadrantIndex)
		{
			SolvedChildQuadrants[QuadrantIndex] = ConstructBoardWithCenteredQuadrant((ChildNode) QuadrantIndex)->GetNextGeneration();
		});
	*/
	mRootNode = QuadTreeNode::CreateNodeWithSubnodes(mMaxLevelInTree, ConstructBoardWithCenteredQuadrant(ChildNode::Northwest)->GetNextGeneration(), ConstructBoardWithCenteredQuadrant(ChildNode::Northeast)->GetNextGeneration(), ConstructBoardWithCenteredQuadrant(ChildNode::Southwest)->GetNextGeneration(), ConstructBoardWithCenteredQuadrant(ChildNode::Southeast)->GetNextGeneration());
	//mRootNode = QuadTreeNode::CreateNodeWithSubnodes(mMaxLevelInTree, SolvedChildQuadrants[0], SolvedChildQuadrants[1], SolvedChildQuadrants[2], SolvedChildQuadrants[3]);
}

FString UGameBoard::GetBoardStringForBlockOfDimensionContainingCoordinate(uint64 DesiredDimension, const FBoardCoordinate Coordinate) const
{
	TSharedPtr<const QuadTreeNode> FoundBlock = mRootNode->GetBlockOfDimensionContainingCoordinate(DesiredDimension, Coordinate.mX, Coordinate.mY);

	return FoundBlock->GetNodeString();
}

void UGameBoard::GetLocalLiveCellCoordinatesFromFoundBlock(uint64 DesiredDimensionOfBlock, const FBoardCoordinate CoordinateToFind, TArray<FBoardCoordinate>& ResultsOut) const
{
	TSharedPtr<const QuadTreeNode> FoundBlock = mRootNode->GetBlockOfDimensionContainingCoordinate(DesiredDimensionOfBlock, CoordinateToFind.mX, CoordinateToFind.mY);

	const uint64 BlockDimension = FoundBlock->GetNodeDimension();

	// Go through the block and look for live cells.
	// This could be slow, but we should only be doing this for a small number of blocks at a time.
	for (uint64 YIter = 0; YIter < BlockDimension; ++YIter)
	{
		for (uint64 XIter = 0; XIter < BlockDimension; ++XIter)
		{
			if (FoundBlock->GetIsCellAlive(XIter, YIter))
			{
				FBoardCoordinate Coordinate;
				Coordinate.SetXAndY(XIter, YIter);
				
				ResultsOut.Add(Coordinate);
			}
		}
	}
}

TSharedPtr<const QuadTreeNode> UGameBoard::GetBlockOfDimensionContainingCoordinate(uint64 DesiredDimensionOfBlock, uint64 X, uint64 Y) const
{
	return mRootNode->GetBlockOfDimensionContainingCoordinate(DesiredDimensionOfBlock, X, Y);
}

FString UGameBoard::GetBoardString() const
{
	return mRootNode->GetNodeString();
}