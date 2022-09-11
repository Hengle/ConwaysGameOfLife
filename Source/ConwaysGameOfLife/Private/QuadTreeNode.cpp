// Fill out your copyright notice in the Description page of Project Settings.

#include "QuadTreeNode.h"
#include "QuadTreeLeaf.h"

TSharedPtr<const QuadTreeNode> QuadTreeNode::CreateEmptyNode(const uint8 NumLevels)
{
	if (NumLevels == 0)
	{
		return MakeShareable<QuadTreeNode>(new QuadTreeLeaf(false));
	}

	TSharedPtr<const QuadTreeNode> EmptyChild = CreateEmptyNode(NumLevels - 1);

	return CreateNodeWithSubnodes(NumLevels, EmptyChild, EmptyChild, EmptyChild, EmptyChild);
}

TSharedPtr<const QuadTreeNode> QuadTreeNode::CreateNodeWithSubnodes(const uint8 Level, const TSharedPtr<const QuadTreeNode> Northwest, const TSharedPtr<const QuadTreeNode> Northeast, const TSharedPtr<const QuadTreeNode> Southwest, const TSharedPtr<const QuadTreeNode> Southeast)
{
	if (Level == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("We're trying to construct a node with subnodes at level 0, this is not supported."));
	}

	return MakeShareable<QuadTreeNode>(new QuadTreeNode(Level, Northwest, Northeast, Southwest, Southeast));
}

TSharedPtr<const QuadTreeLeaf> QuadTreeNode::GetNextGenerationCellFromNeighborhood(uint16 NeighborhoodBitset)
{
	if (NeighborhoodBitset == 0)
	{
		return MakeShareable<QuadTreeLeaf>(new QuadTreeLeaf(false));
	}

	const bool IsCurrentCellAlive = (NeighborhoodBitset >> 5) & 0x1;

	const uint16 Bitmask = 0x757;
	NeighborhoodBitset &= Bitmask;

	uint8 NeighborCount = 0;

	while (NeighborhoodBitset > 0)
	{
		++NeighborCount;
		NeighborhoodBitset &= (NeighborhoodBitset - 1);
	}

	if (NeighborCount == 3 || (IsCurrentCellAlive && NeighborCount == 2))
	{
		return MakeShareable<QuadTreeLeaf>(new QuadTreeLeaf(true));
	}

	return MakeShareable<QuadTreeLeaf>(new QuadTreeLeaf(false));
}

QuadTreeNode::QuadTreeNode(const uint8 Level) :
	mLevel(Level)
{

}

QuadTreeNode::QuadTreeNode(const uint8 Level, const TSharedPtr<const QuadTreeNode> Northwest, const TSharedPtr<const QuadTreeNode> Northeast, const TSharedPtr<const QuadTreeNode> Southwest, const TSharedPtr<const QuadTreeNode> Southeast) :
	mLevel(Level)
{
	mChildren[ChildNode::Northwest] = Northwest;
	mChildren[ChildNode::Northeast] = Northeast;
	mChildren[ChildNode::Southwest] = Southwest;
	mChildren[ChildNode::Southeast] = Southeast;
}

ChildNode QuadTreeNode::GetChildAndLocalCoordinates(const int64 X, const int64 Y, int64& LocalXOut, int64& LocalYOut) const
{
	const int64 ChildBlockSize = pow(2, mLevel - 1);

	LocalXOut = X;
	LocalYOut = Y;

	if (X < ChildBlockSize)
	{
		if (Y < ChildBlockSize)
		{
			return ChildNode::Northwest;
		}
		else
		{
			LocalYOut = Y - ChildBlockSize;
			
			return ChildNode::Southwest;
		}
	}
	else // X >= ChildBlockSize
	{
		if (Y < ChildBlockSize)
		{
			LocalXOut = X - ChildBlockSize;
			
			return ChildNode::Northeast;
		}
		else
		{
			LocalXOut = X - ChildBlockSize;
			LocalYOut = Y - ChildBlockSize;

			return ChildNode::Southeast;
		}
	}
}

bool QuadTreeNode::GetIsCellAlive(const int64 X, const int64 Y) const
{
	int64 ChildLocalX, ChildLocalY;
	const ChildNode ChildContainingXAndY = GetChildAndLocalCoordinates(X, Y, ChildLocalX, ChildLocalY);

	const TSharedPtr<const QuadTreeNode> Child = mChildren[ChildContainingXAndY];
	if (Child.IsValid())
	{
		return Child->GetIsCellAlive(ChildLocalX, ChildLocalY);
	}
	
	return false;
}

TSharedPtr<const QuadTreeNode> QuadTreeNode::SetCellToAlive(const int64 X, const int64 Y) const
{
	int64 ChildLocalX, ChildLocalY;
	const ChildNode ChildContainingXAndY = GetChildAndLocalCoordinates(X, Y, ChildLocalX, ChildLocalY);

	TSharedPtr<const QuadTreeNode> NewChild = mChildren[ChildContainingXAndY];
	if (!NewChild.IsValid())
	{
		// Will want to use our static creation functions here for sure.
		NewChild = CreateEmptyNode(mLevel - 1);
	}

	NewChild = NewChild->SetCellToAlive(ChildLocalX, ChildLocalY);

	// Also switch to static creation functions here!
	switch (ChildContainingXAndY)
	{
	case ChildNode::Northwest:
		return MakeShareable<QuadTreeNode>(new QuadTreeNode(mLevel, NewChild, GetChild(ChildNode::Northeast), GetChild(ChildNode::Southwest), GetChild(ChildNode::Southeast)));
	case ChildNode::Northeast:
		return MakeShareable<QuadTreeNode>(new QuadTreeNode(mLevel, GetChild(ChildNode::Northwest), NewChild, GetChild(ChildNode::Southwest), GetChild(ChildNode::Southeast)));
	case ChildNode::Southwest:
		return MakeShareable<QuadTreeNode>(new QuadTreeNode(mLevel, GetChild(ChildNode::Northwest), GetChild(ChildNode::Northeast), NewChild, GetChild(ChildNode::Southeast)));
	case ChildNode::Southeast:
		return MakeShareable<QuadTreeNode>(new QuadTreeNode(mLevel, GetChild(ChildNode::Northwest), GetChild(ChildNode::Northeast), GetChild(ChildNode::Southwest), NewChild));
	}

	return TSharedPtr<QuadTreeNode>(nullptr);
}

TSharedPtr<const QuadTreeNode> QuadTreeNode::GetChild(ChildNode Node) const
{
	return mChildren[Node];
}

bool QuadTreeNode::IsAnyChildValid() const
{
	for (int i = 0; i < ChildNode::kCount; ++i)
	{
		if (mChildren[i].IsValid())
		{
			return true;
		}
	}

	return false;
}

TSharedPtr<const QuadTreeNode> QuadTreeNode::Run4x4Simulation() const
{
	if (GetBlockDimension() != 4)
	{
		UE_LOG(LogTemp, Warning, TEXT("We're calling Run4x4Simulation() on a node that isn't 4x4."));
	}

	// Create bitmask to store all the neighbors.
	uint16 Bitmask = 0;

	for (int YIter = 0; YIter < 4; ++YIter)
	{
		for (int XIter = 0; XIter < 4; ++XIter)
		{
			Bitmask = (Bitmask << 1) + GetIsCellAlive(XIter, YIter);
		}
	}

	return CreateNodeWithSubnodes(mLevel - 1, GetNextGenerationCellFromNeighborhood(Bitmask >> 5), GetNextGenerationCellFromNeighborhood(Bitmask >> 4), GetNextGenerationCellFromNeighborhood(Bitmask >> 1), GetNextGenerationCellFromNeighborhood(Bitmask));
}

TSharedPtr<const QuadTreeNode> QuadTreeNode::GetNextGeneration() const
{
	if (!IsAnyChildValid())
	{
		return CreateEmptyNode(mLevel - 1);
	}
	else if (GetBlockDimension() == 4)
	{
		return Run4x4Simulation();
	}

	TSharedPtr<const QuadTreeNode> CenterNorthwest, CenterNorth, CenterNortheast, CenterWest, TrueCenter, CenterEast, CenterSouthwest, CenterSouth, CenterSoutheast;

	CenterNorthwest = GetChild(ChildNode::Northwest)->ConstructCenteredChild();
	CenterNorth = ConstructHorizontalCenteredGrandchild(GetChild(ChildNode::Northwest), GetChild(ChildNode::Northeast));
	CenterNortheast = GetChild(ChildNode::Northeast)->ConstructCenteredChild();
	
	CenterWest = ConstructVerticalCenteredGrandchild(GetChild(ChildNode::Northwest), GetChild(ChildNode::Southwest));
	TrueCenter = ConstructCenteredGrandchild();
	CenterEast = ConstructVerticalCenteredGrandchild(GetChild(ChildNode::Northeast), GetChild(ChildNode::Southeast));
	
	CenterSouthwest = GetChild(ChildNode::Southwest)->ConstructCenteredChild();
	CenterSouth = ConstructHorizontalCenteredGrandchild(GetChild(ChildNode::Southwest), GetChild(ChildNode::Southeast));
	CenterSoutheast = GetChild(ChildNode::Southeast)->ConstructCenteredChild();

	return CreateNodeWithSubnodes(mLevel - 1,
		CreateNodeWithSubnodes(mLevel - 1, CenterNorthwest, CenterNorth, CenterWest, TrueCenter)->GetNextGeneration(),
		CreateNodeWithSubnodes(mLevel - 1, CenterNorth, CenterNortheast, TrueCenter, CenterEast)->GetNextGeneration(),
		CreateNodeWithSubnodes(mLevel - 1, CenterWest, TrueCenter, CenterSouthwest, CenterSouth)->GetNextGeneration(),
		CreateNodeWithSubnodes(mLevel - 1, TrueCenter, CenterEast, CenterSouth, CenterSoutheast)->GetNextGeneration());
}

TSharedPtr<const QuadTreeNode> QuadTreeNode::ConstructCenteredChild() const
{
	// Construct a node at (mLevel - 1) that consists of the cells in the center of this node. 
	return CreateNodeWithSubnodes(mLevel - 1,
		GetChild(ChildNode::Northwest)->GetChild(ChildNode::Southeast),
		GetChild(ChildNode::Northeast)->GetChild(ChildNode::Southwest),
		GetChild(ChildNode::Southwest)->GetChild(ChildNode::Northeast),
		GetChild(ChildNode::Southeast)->GetChild(ChildNode::Northwest));
}

int64 QuadTreeNode::GetBlockDimension() const
{
	return pow(2, mLevel);
}

TSharedPtr<const QuadTreeNode> QuadTreeNode::ConstructHorizontalCenteredGrandchild(TSharedPtr<const QuadTreeNode> WestChildNode, TSharedPtr<const QuadTreeNode> EastChildNode) const 
{
	if (mLevel < 3)
	{
		UE_LOG(LogTemp, Warning, TEXT("Attempting to construct the grandchild of a node with mLevel < 3. We do not have adequate data to do so."));
	}

	TSharedPtr<const QuadTreeNode> NorthwestSubnode, NortheastSubnode, SouthwestSubnode, SoutheastSubnode;

	NorthwestSubnode = WestChildNode->GetChild(ChildNode::Northeast)->GetChild(ChildNode::Southeast);
	NortheastSubnode = EastChildNode->GetChild(ChildNode::Northwest)->GetChild(ChildNode::Southwest);
	SouthwestSubnode = WestChildNode->GetChild(ChildNode::Southeast)->GetChild(ChildNode::Northeast);
	SoutheastSubnode = EastChildNode->GetChild(ChildNode::Southwest)->GetChild(ChildNode::Northwest);

	return CreateNodeWithSubnodes(mLevel - 2, NorthwestSubnode, NortheastSubnode, SouthwestSubnode, SoutheastSubnode);
}

TSharedPtr<const QuadTreeNode> QuadTreeNode::ConstructVerticalCenteredGrandchild(TSharedPtr<const QuadTreeNode> NorthChildNode, TSharedPtr<const QuadTreeNode> SouthChildNode) const
{
	if (mLevel < 3)
	{
		UE_LOG(LogTemp, Warning, TEXT("Attempting to construct the grandchild of a node with mLevel < 3. We do not have adequate data to do so."));
	}

	TSharedPtr<const QuadTreeNode> NorthwestSubnode, NortheastSubnode, SouthwestSubnode, SoutheastSubnode;

	NorthwestSubnode = NorthChildNode->GetChild(ChildNode::Southwest)->GetChild(ChildNode::Southeast);
	NortheastSubnode = NorthChildNode->GetChild(ChildNode::Southeast)->GetChild(ChildNode::Southwest);
	SouthwestSubnode = SouthChildNode->GetChild(ChildNode::Northwest)->GetChild(ChildNode::Northeast);
	SoutheastSubnode = SouthChildNode->GetChild(ChildNode::Northeast)->GetChild(ChildNode::Northwest);

	return CreateNodeWithSubnodes(mLevel - 2, NorthwestSubnode, NortheastSubnode, SouthwestSubnode, SoutheastSubnode);
}

TSharedPtr<const QuadTreeNode> QuadTreeNode::ConstructCenteredGrandchild() const
{
	if (mLevel < 3)
	{
		UE_LOG(LogTemp, Warning, TEXT("Attempting to construct the grandchild of a node with mLevel < 3. We do not have adequate data to do so."));
	}

	TSharedPtr<const QuadTreeNode> NorthwestSubnode, NortheastSubnode, SouthwestSubnode, SoutheastSubnode;

	NorthwestSubnode = GetChild(ChildNode::Northwest)->GetChild(ChildNode::Southeast)->GetChild(ChildNode::Southeast);
	NortheastSubnode = GetChild(ChildNode::Northeast)->GetChild(ChildNode::Southwest)->GetChild(ChildNode::Southwest);
	SouthwestSubnode = GetChild(ChildNode::Southwest)->GetChild(ChildNode::Northeast)->GetChild(ChildNode::Northeast);
	SoutheastSubnode = GetChild(ChildNode::Southeast)->GetChild(ChildNode::Northwest)->GetChild(ChildNode::Northwest);

	return CreateNodeWithSubnodes(mLevel - 2, NorthwestSubnode, NortheastSubnode, SouthwestSubnode, SoutheastSubnode);
}