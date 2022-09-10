// Fill out your copyright notice in the Description page of Project Settings.

#include "QuadTreeNode.h"
#include "QuadTreeLeaf.h"

TSharedPtr<const QuadTreeNode> QuadTreeNode::CreateEmptyNode(const uint8 NumLevels)
{
	if (NumLevels == 0)
	{
		return MakeShareable<QuadTreeNode>(new QuadTreeLeaf(false));
	}

	return MakeShareable<QuadTreeNode>(new QuadTreeNode(NumLevels));
}

TSharedPtr<const QuadTreeNode> QuadTreeNode::CreateNodeWithSubnodes(const uint8 Level, const TSharedPtr<const QuadTreeNode> Northwest, const TSharedPtr<const QuadTreeNode> Northeast, const TSharedPtr<const QuadTreeNode> Southwest, const TSharedPtr<const QuadTreeNode> Southeast)
{
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

bool QuadTreeNode::GetBit(const int64 X, const int64 Y) const
{
	int64 ChildLocalX, ChildLocalY;
	const ChildNode ChildContainingXAndY = GetChildAndLocalCoordinates(X, Y, ChildLocalX, ChildLocalY);

	const TSharedPtr<const QuadTreeNode> Child = mChildren[ChildContainingXAndY];
	if (Child.IsValid())
	{
		return Child->GetBit(ChildLocalX, ChildLocalY);
	}
	
	return false;
}

TSharedPtr<const QuadTreeNode> QuadTreeNode::SetBit(const int64 X, const int64 Y) const
{
	int64 ChildLocalX, ChildLocalY;
	const ChildNode ChildContainingXAndY = GetChildAndLocalCoordinates(X, Y, ChildLocalX, ChildLocalY);

	TSharedPtr<const QuadTreeNode> NewChild = mChildren[ChildContainingXAndY];
	if (!NewChild.IsValid())
	{
		// Will want to use our static creation functions here for sure.
		NewChild = CreateEmptyNode(mLevel - 1);
	}

	NewChild = NewChild->SetBit(ChildLocalX, ChildLocalY);

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
	if (mLevel != 2)
	{
		UE_LOG(LogTemp, Warning, TEXT("We're calling Run4x4Simulation() in a node that isn't 4x4."));
	}

	// Create bitmask to store all the neighbors.
	uint16 Bitmask = 0;

	for (int YIter = 0; YIter < 4; ++YIter)
	{
		for (int XIter = 0; XIter < 4; ++XIter)
		{
			Bitmask = (Bitmask << 1) + GetBit(XIter, YIter);
		}
	}

	return CreateNodeWithSubnodes(mLevel, GetNextGenerationCellFromNeighborhood(Bitmask >> 5), GetNextGenerationCellFromNeighborhood(Bitmask >> 4), GetNextGenerationCellFromNeighborhood(Bitmask >> 1), GetNextGenerationCellFromNeighborhood(Bitmask));
}

TSharedPtr<const QuadTreeNode> QuadTreeNode::GetNextGeneration() const
{
	if (!IsAnyChildValid())
	{
		return CreateEmptyNode(mLevel - 1);
	}
	else if (mLevel == 2)
	{
		return Run4x4Simulation();
	}

	return mChildren[0]->GetNextGeneration();
}