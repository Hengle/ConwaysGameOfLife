// Conway's Game Of Life in Unreal
// Ilana Franklin, 2022

#include "QuadTreeNode.h"

// Initialization for our canonical leaf nodes. Having canonical versions of these will cut down on memory requirements.
TSharedPtr<const QuadTreeNode> QuadTreeNode::sCanonicalLiveCell = MakeShareable<QuadTreeNode>(new QuadTreeNode(true));
TSharedPtr<const QuadTreeNode> QuadTreeNode::sCanonicalDeadCell = MakeShareable<QuadTreeNode>(new QuadTreeNode(false));

TSharedPtr<const QuadTreeNode> QuadTreeNode::CreateLeaf(bool IsAlive)
{
	if (IsAlive)
	{
		return sCanonicalLiveCell;
	}

	return sCanonicalDeadCell;
}

TSharedPtr<const QuadTreeNode> QuadTreeNode::CreateNodeWithSubnodes(const uint8 Level, const TSharedPtr<const QuadTreeNode> Northwest, const TSharedPtr<const QuadTreeNode> Northeast, const TSharedPtr<const QuadTreeNode> Southwest, const TSharedPtr<const QuadTreeNode> Southeast)
{
#if !UE_BUILD_SHIPPING
	if (!Northwest.IsValid() || !Northeast.IsValid() || !Southwest.IsValid() || !Southeast.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Attempting to create a node with some non-valid subnode."));
		return nullptr;
	}

	if (Northwest->mLevel != Level - 1 || Northeast->mLevel != Level - 1 || Southwest->mLevel != Level - 1 || Southeast->mLevel != Level - 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("Attempting to create a node with subnodes that are not at the correct level."));
		return nullptr;
	}

	if (Level == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("We're trying to construct a node with subnodes at level 0, this is not supported."));
		return nullptr;
	}
#endif

	return MakeShareable<QuadTreeNode>(new QuadTreeNode(Level, Northwest, Northeast, Southwest, Southeast));
}

TSharedPtr<const QuadTreeNode> QuadTreeNode::CreateEmptyNode(const uint8 NumLevels)
{
	if (NumLevels == 0)
	{
		return CreateLeaf(false);
	}

	TSharedPtr<const QuadTreeNode> EmptyChild = CreateEmptyNode(NumLevels - 1);

	return CreateNodeWithSubnodes(NumLevels, EmptyChild, EmptyChild, EmptyChild, EmptyChild);
}

TSharedPtr<const QuadTreeNode> QuadTreeNode::GetNextGenerationCellFromNeighborhood(uint16 NeighborhoodBitset)
{
	if (NeighborhoodBitset == 0)
	{
		return CreateLeaf(false);
	}

	const bool IsCurrentCellAlive = (NeighborhoodBitset >> 5) & 0x1;

	// Mask off bits outside the neighborhood, and the cell itself.
	const uint16 Bitmask = 0x757;
	NeighborhoodBitset &= Bitmask;

	// Count the number of 1s in the neighborhood bitset. This represents our live neighbors.
	uint8 NeighborCount = 0;

	while (NeighborhoodBitset > 0)
	{
		++NeighborCount;
		NeighborhoodBitset &= (NeighborhoodBitset - 1);
	}

	// Apply our Game of Life rules.
	if (NeighborCount == 3 || (IsCurrentCellAlive && NeighborCount == 2))
	{
		return CreateLeaf(true);
	}

	return CreateLeaf(false);
}

QuadTreeNode::QuadTreeNode(const bool IsAlive) :
	mLevel(0),
	mIsAlive(IsAlive)
{

}

QuadTreeNode::QuadTreeNode(const uint8 Level, const TSharedPtr<const QuadTreeNode> Northwest, const TSharedPtr<const QuadTreeNode> Northeast, const TSharedPtr<const QuadTreeNode> Southwest, const TSharedPtr<const QuadTreeNode> Southeast) :
	mLevel(Level)
{
	mChildren[ChildNode::Northwest] = Northwest;
	mChildren[ChildNode::Northeast] = Northeast;
	mChildren[ChildNode::Southwest] = Southwest;
	mChildren[ChildNode::Southeast] = Southeast;

	// This node is alive if at least one cell inside it is alive.
	mIsAlive = (Northwest->IsAlive() || Northeast->IsAlive() || Southwest->IsAlive() || Southeast->IsAlive());
}

bool QuadTreeNode::operator==(const QuadTreeNode& Other) const
{
	return (mLevel == Other.mLevel) &&
		(mIsAlive == mIsAlive) &&
		(Northwest() == Other.Northwest()) &&
		(Northeast() == Other.Northeast()) &&
		(Southwest() == Other.Southwest()) &&
		(Southeast() == Other.Southeast());
}

ChildNode QuadTreeNode::GetChildAndLocalCoordinates(const uint64 X, const uint64 Y, uint64& LocalXOut, uint64& LocalYOut) const
{
#if !UE_BUILD_SHIPPING
	if (IsLeaf())
	{
		UE_LOG(LogTemp, Warning, TEXT("Should not be calling GetChildAndLocalCoordinates on a leaf node."));
		return ChildNode::Northwest;
	}
#endif

	const uint64 ChildNodeDimension = pow(2, mLevel - 1);

	LocalXOut = X;
	LocalYOut = Y;

	if (X < ChildNodeDimension)
	{
		if (Y < ChildNodeDimension)
		{
			return ChildNode::Southwest;
		}
		else
		{
			LocalYOut = Y - ChildNodeDimension;
			
			return ChildNode::Northwest;
		}
	}
	else
	{
		LocalXOut = X - ChildNodeDimension;

		if (Y < ChildNodeDimension)
		{
			return ChildNode::Southeast;
		}
		else
		{
			LocalYOut = Y - ChildNodeDimension;

			return ChildNode::Northeast;
		}
	}
}

bool QuadTreeNode::GetIsCellAlive(const uint64 X, const uint64 Y) const
{
	if (IsLeaf())
	{
		return IsAlive();
	}

	uint64 ChildLocalX, ChildLocalY;
	const ChildNode ChildContainingXAndY = GetChildAndLocalCoordinates(X, Y, ChildLocalX, ChildLocalY);

	const TSharedPtr<const QuadTreeNode> Child = mChildren[ChildContainingXAndY];
#if !UE_BUILD_SHIPPING
	if (!Child.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Child node was not valid when attempting to call GetIsCellAlive."));
		return false;
	}
#endif

	return Child->GetIsCellAlive(ChildLocalX, ChildLocalY);
}

TSharedPtr<const QuadTreeNode> QuadTreeNode::SetCellToAlive(const uint64 X, const uint64 Y) const
{
	if (IsLeaf())
	{
		return CreateLeaf(true);
	}

	uint64 ChildLocalX, ChildLocalY;
	const ChildNode ChildContainingXAndY = GetChildAndLocalCoordinates(X, Y, ChildLocalX, ChildLocalY);

	TSharedPtr<const QuadTreeNode> NewChild = mChildren[ChildContainingXAndY];
#if !UE_BUILD_SHIPPING
	if (!NewChild.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Child node was not valid when attempting to call SetCellToAlive."));
		return nullptr;
	}
#endif

	NewChild = NewChild->SetCellToAlive(ChildLocalX, ChildLocalY);

	// Depending on which child contains the cell we'd like to set, return the appropriate new tree with a new child. 
	switch (ChildContainingXAndY)
	{
	case ChildNode::Northwest:
		return CreateNodeWithSubnodes(mLevel, NewChild, Northeast(), Southwest(), Southeast());
	case ChildNode::Northeast:
		return CreateNodeWithSubnodes(mLevel, Northwest(), NewChild, Southwest(), Southeast());
	case ChildNode::Southwest:
		return CreateNodeWithSubnodes(mLevel, Northwest(), Northeast(), NewChild, Southeast());
	case ChildNode::Southeast:
		return CreateNodeWithSubnodes(mLevel, Northwest(), Northeast(), Southwest(), NewChild);
	}

	return TSharedPtr<QuadTreeNode>(nullptr);
}

TSharedPtr<const QuadTreeNode> QuadTreeNode::GetChild(ChildNode Node) const
{
#if !UE_BUILD_SHIPPING
	if (IsLeaf())
	{
		UE_LOG(LogTemp, Warning, TEXT("Attempting to call GetChild on a leaf."));
		return nullptr;
	}
#endif
	return mChildren[Node];
}

TSharedPtr<const QuadTreeNode> QuadTreeNode::Run4x4Simulation() const
{
#if !UE_BUILD_SHIPPING
	if (GetNodeDimension() != 4)
	{
		UE_LOG(LogTemp, Warning, TEXT("We're calling Run4x4Simulation() on a node that isn't 4x4."));
		return nullptr;
	}
#endif

	// Create bitset to store all the neighbors.
	uint16 Bitset = 0;

	// Place all neighbors in this 4x4 bitset, starting in the upper left and going row by row.
	for (uint64 YIter = 4; YIter > 0; --YIter)
	{
		for (uint64 XIter = 0; XIter < 4; ++XIter)
		{
			// Append either a 1 or a 0 depending on whether or not the cell is alive.
			Bitset = (Bitset << 1) + GetIsCellAlive(XIter, YIter - 1);
		}
	}

	// Using some known Game Of Life bitset tricks, we can quickly calculate the next generation of the interior 2x2 node.
	return CreateNodeWithSubnodes(mLevel - 1, GetNextGenerationCellFromNeighborhood(Bitset >> 5), GetNextGenerationCellFromNeighborhood(Bitset >> 4), GetNextGenerationCellFromNeighborhood(Bitset >> 1), GetNextGenerationCellFromNeighborhood(Bitset));
}

TSharedPtr<const QuadTreeNode> QuadTreeNode::GetNextGeneration() const
{
	if (!IsAlive())
	{
		// If there are no live cells in this node, we can just return an empty tree.
		return CreateEmptyNode(mLevel - 1);
	}
	else if (GetNodeDimension() == 4)
	{
		// Once we've reached a 4x4 block, go to our specialized simulation.
		return Run4x4Simulation();
	}

	/*
	 * Our goal is to return a node at level (mLevel-1) which represents how a centered child node would look if advanced one generation. 
	 * We're going to construct 9 interior nodes, each at level (mLevel-2). These nodes surround and overlap with our intended centered result node.
	 * Using the 9 interior nodes we can construct four new trees, each of which have one quadrant of the intended centered result node in their own center.
	 * Advancing each of these trees by one generation will give us one quadrant of our intended centered result node.
	 * We can then combine these four solved quadrants to form our result node.
	 */
	TSharedPtr<const QuadTreeNode> CenterNorthwest, CenterNorth, CenterNortheast, CenterWest, TrueCenter, CenterEast, CenterSouthwest, CenterSouth, CenterSoutheast;

	CenterNorthwest = Northwest()->ConstructCenteredChild();
	CenterNorth = ConstructHorizontalCenteredGrandchild(Northwest(), Northeast());
	CenterNortheast = Northeast()->ConstructCenteredChild();
	
	CenterWest = ConstructVerticalCenteredGrandchild(Northwest(), Southwest());
	TrueCenter = ConstructCenteredGrandchild();
	CenterEast = ConstructVerticalCenteredGrandchild(Northeast(), Southeast());
	
	CenterSouthwest = Southwest()->ConstructCenteredChild();
	CenterSouth = ConstructHorizontalCenteredGrandchild(Southwest(), Southeast());
	CenterSoutheast = Southeast()->ConstructCenteredChild();

	// Construct our four nodes that will give us each of our four quadrants for the intended centered result node and simulate their next generation in parallel. 
	TSharedPtr<const QuadTreeNode> NewNorthwest, NewNortheast, NewSouthwest, NewSoutheast;

	/*
	ParallelFor(ChildNode::kCount, [&](int32 QuadrantIndex)
		{
			switch (QuadrantIndex) 
			{
			case ChildNode::Northwest:
				NewNorthwest = CreateNodeWithSubnodes(mLevel - 1, CenterNorthwest, CenterNorth, CenterWest, TrueCenter)->GetNextGeneration();
				break;
			case ChildNode::Northeast:
				NewNortheast = CreateNodeWithSubnodes(mLevel - 1, CenterNorth, CenterNortheast, TrueCenter, CenterEast)->GetNextGeneration();
				break;
			case ChildNode::Southwest:
				NewSouthwest = CreateNodeWithSubnodes(mLevel - 1, CenterWest, TrueCenter, CenterSouthwest, CenterSouth)->GetNextGeneration();
				break;
			case ChildNode::Southeast:
				NewSoutheast = CreateNodeWithSubnodes(mLevel - 1, TrueCenter, CenterEast, CenterSouth, CenterSoutheast)->GetNextGeneration();
				break;
			default:
				UE_LOG(LogTemp, Warning, TEXT("Reached some unknown case during ParallelFor in GetNextGeneration."))
			}
		});

	*/

	for (int i = 0; i < ChildNode::kCount; ++i)
	{
		switch (i)
		{
		case ChildNode::Northwest:
			NewNorthwest = CreateNodeWithSubnodes(mLevel - 1, CenterNorthwest, CenterNorth, CenterWest, TrueCenter)->GetNextGeneration();
			break;
		case ChildNode::Northeast:
			NewNortheast = CreateNodeWithSubnodes(mLevel - 1, CenterNorth, CenterNortheast, TrueCenter, CenterEast)->GetNextGeneration();
			break;
		case ChildNode::Southwest:
			NewSouthwest = CreateNodeWithSubnodes(mLevel - 1, CenterWest, TrueCenter, CenterSouthwest, CenterSouth)->GetNextGeneration();
			break;
		case ChildNode::Southeast:
			NewSoutheast = CreateNodeWithSubnodes(mLevel - 1, TrueCenter, CenterEast, CenterSouth, CenterSoutheast)->GetNextGeneration();
			break;
		default:
			UE_LOG(LogTemp, Warning, TEXT("Reached some unknown case during ParallelFor in GetNextGeneration."))
		}
	}



	// Recombine to get our centered result node.
	return CreateNodeWithSubnodes(mLevel - 1, NewNorthwest, NewNortheast, NewSouthwest, NewSoutheast);
}

TSharedPtr<const QuadTreeNode> QuadTreeNode::ConstructCenteredChild() const
{
	// Construct a node at (mLevel - 1) that consists of the cells in the center of this node. 
	return CreateNodeWithSubnodes(mLevel - 1,
		Northwest()->Southeast(),
		Northeast()->Southwest(),
		Southwest()->Northeast(),
		Southeast()->Northwest());
}

uint64 QuadTreeNode::GetNodeDimension() const
{
	return pow(2, mLevel);
}

FString QuadTreeNode::GetNodeString() const
{
	FString Result = "\n";

	uint64 Dimension = GetNodeDimension();

	for (uint64 YIter = Dimension; YIter > 0; --YIter)
	{
		for (uint64 XIter = 0; XIter < Dimension; ++XIter)
		{
			if (GetIsCellAlive(XIter, YIter - 1))
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

TSharedPtr<const QuadTreeNode> QuadTreeNode::Northwest() const
{
	return GetChild(ChildNode::Northwest);
}

TSharedPtr<const QuadTreeNode> QuadTreeNode::Northeast() const
{
	return GetChild(ChildNode::Northeast);
}

TSharedPtr<const QuadTreeNode> QuadTreeNode::Southwest() const
{
	return GetChild(ChildNode::Southwest);
}

TSharedPtr<const QuadTreeNode> QuadTreeNode::Southeast() const
{
	return GetChild(ChildNode::Southeast);
}

bool QuadTreeNode::IsLeaf() const
{
	// All leaves are at level 0, and all nodes at level 0 are leaves.
	return mLevel == 0;
}

bool QuadTreeNode::IsAlive() const
{
	return mIsAlive;
}

TSharedPtr<const QuadTreeNode> QuadTreeNode::GetBlockOfDimensionContainingCoordinate(const uint64 DesiredDimension, const uint64 X, const uint64 Y) const
{
	if (GetNodeDimension() == DesiredDimension)
	{
		return CreateNodeWithSubnodes(mLevel, Northwest(), Northeast(), Southwest(), Southeast());
	}
	else if (GetNodeDimension() < DesiredDimension)
	{
		UE_LOG(LogTemp, Error, TEXT("Could not find any block with the desired dimension. DesiredDimension must be a power of two to find a block successfully."));
		return nullptr;
	}

	uint64 ChildLocalX, ChildLocalY;
	const ChildNode ChildContainingXAndY = GetChildAndLocalCoordinates(X, Y, ChildLocalX, ChildLocalY);

	return GetChild(ChildContainingXAndY)->GetBlockOfDimensionContainingCoordinate(DesiredDimension, ChildLocalX, ChildLocalY);
}

TSharedPtr<const QuadTreeNode> QuadTreeNode::ConstructHorizontalCenteredGrandchild(TSharedPtr<const QuadTreeNode> WestChildNode, TSharedPtr<const QuadTreeNode> EastChildNode) const 
{
#if !UE_BUILD_SHIPPING
	if (mLevel < 3)
	{
		UE_LOG(LogTemp, Warning, TEXT("Attempting to construct the grandchild of a node with mLevel < 3. We do not have adequate data to do so."));
	}
#endif

	TSharedPtr<const QuadTreeNode> NorthwestSubnode, NortheastSubnode, SouthwestSubnode, SoutheastSubnode;

	NorthwestSubnode = WestChildNode->Northeast()->Southeast();
	NortheastSubnode = EastChildNode->Northwest()->Southwest();
	SouthwestSubnode = WestChildNode->Southeast()->Northeast();
	SoutheastSubnode = EastChildNode->Southwest()->Northwest();

	return CreateNodeWithSubnodes(mLevel - 2, NorthwestSubnode, NortheastSubnode, SouthwestSubnode, SoutheastSubnode);
}

TSharedPtr<const QuadTreeNode> QuadTreeNode::ConstructVerticalCenteredGrandchild(TSharedPtr<const QuadTreeNode> NorthChildNode, TSharedPtr<const QuadTreeNode> SouthChildNode) const
{
#if !UE_BUILD_SHIPPING
	if (mLevel < 3)
	{
		UE_LOG(LogTemp, Warning, TEXT("Attempting to construct the grandchild of a node with mLevel < 3. We do not have adequate data to do so."));
	}
#endif

	TSharedPtr<const QuadTreeNode> NorthwestSubnode, NortheastSubnode, SouthwestSubnode, SoutheastSubnode;

	NorthwestSubnode = NorthChildNode->Southwest()->Southeast();
	NortheastSubnode = NorthChildNode->Southeast()->Southwest();
	SouthwestSubnode = SouthChildNode->Northwest()->Northeast();
	SoutheastSubnode = SouthChildNode->Northeast()->Northwest();

	return CreateNodeWithSubnodes(mLevel - 2, NorthwestSubnode, NortheastSubnode, SouthwestSubnode, SoutheastSubnode);
}

TSharedPtr<const QuadTreeNode> QuadTreeNode::ConstructCenteredGrandchild() const
{
#if !UE_BUILD_SHIPPING
	if (mLevel < 3)
	{
		UE_LOG(LogTemp, Warning, TEXT("Attempting to construct the grandchild of a node with mLevel < 3. We do not have adequate data to do so."));
	}
#endif

	TSharedPtr<const QuadTreeNode> NorthwestSubnode, NortheastSubnode, SouthwestSubnode, SoutheastSubnode;

	NorthwestSubnode = Northwest()->Southeast()->Southeast();
	NortheastSubnode = Northeast()->Southwest()->Southwest();
	SouthwestSubnode = Southwest()->Northeast()->Northeast();
	SoutheastSubnode = Southeast()->Northwest()->Northwest();

	return CreateNodeWithSubnodes(mLevel - 2, NorthwestSubnode, NortheastSubnode, SouthwestSubnode, SoutheastSubnode);
}