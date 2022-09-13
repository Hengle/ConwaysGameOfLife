// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class QuadTreeLeaf;

enum ChildNode : int8
{
	Northwest = 0,
	Northeast = 1,
	Southwest = 2,
	Southeast = 3,

	kCount = 4
};

enum GrandchildNode : int8
{
	Centered,
	CenteredNorth,
	CenteredSouth,
	CenteredWest,
	CenteredEast
};

/**
 * 
 */
class CONWAYSGAMEOFLIFE_API QuadTreeNode
{
public: 
	static TSharedPtr<const QuadTreeNode> CreateEmptyNode(const uint8 NumLevels);

	static TSharedPtr<const QuadTreeNode> CreateNodeWithSubnodes(const uint8 Level, const TSharedPtr<const QuadTreeNode> Northwest, const TSharedPtr<const QuadTreeNode> Northeast, const TSharedPtr<const QuadTreeNode> Southwest, const TSharedPtr<const QuadTreeNode> Southeast);

	static TSharedPtr<const QuadTreeLeaf> GetNextGenerationCellFromNeighborhood(uint16 NeighborhoodBitset);

public:
	const uint8 mLevel;

	QuadTreeNode(const uint8 Level);
	QuadTreeNode(const uint8 Level, const TSharedPtr<const QuadTreeNode> Northwest, const TSharedPtr<const QuadTreeNode> Northeast, const TSharedPtr<const QuadTreeNode> Southwest, const TSharedPtr<const QuadTreeNode> Southeast);

	// Returns the status of the cell at X and Y, where X and Y are local coordinates in this block.
	virtual bool GetIsCellAlive(const int64 X, const int64 Y) const;

	// Returns a node that is the same as the current node, but with the bit at X and Y set to alive.
	virtual TSharedPtr<const QuadTreeNode> SetCellToAlive(const int64 X, const int64 Y) const;

	TSharedPtr<const QuadTreeNode> GetChild(ChildNode Node) const;

	// Returns a node representing how the centered (mLevel - 1) node would look if advanced one generation.
	virtual TSharedPtr<const QuadTreeNode> GetNextGeneration() const;

	// TODO override this in the child.
	virtual TSharedPtr<const QuadTreeNode> ConstructCenteredChild() const;

	// TODO resolve types here?
	int64 GetBlockDimension() const;

	FString GetNodeString() const;

	TSharedPtr<const QuadTreeNode> Northwest() const;

	TSharedPtr<const QuadTreeNode> Northeast() const;

	TSharedPtr<const QuadTreeNode> Southwest() const;

	TSharedPtr<const QuadTreeNode> Southeast() const;

	bool IsLeaf() const;

private:
	// Pointers to each of our children, which each represent 1/4 of this node's space on the board.
	TStaticArray<TSharedPtr<const QuadTreeNode>, 4> mChildren;

	// Returns the child node that X and Y are contained in. Puts the relative coordinates for X and Y within that child in the out params.
	virtual ChildNode GetChildAndLocalCoordinates(const int64 X, const int64 Y, int64& LocalXOut, int64& LocalYOut) const;

	bool IsAnyChildValid() const;

	// Returns a node representing the centered 2x2 interior square of cells if they were advanced one generation.
	TSharedPtr<const QuadTreeNode> Run4x4Simulation() const;

	TSharedPtr<const QuadTreeNode> ConstructHorizontalCenteredGrandchild(TSharedPtr<const QuadTreeNode> WestChildNode, TSharedPtr<const QuadTreeNode> EastChildNode) const;
	TSharedPtr<const QuadTreeNode> ConstructVerticalCenteredGrandchild(TSharedPtr<const QuadTreeNode> NorthChildNode, TSharedPtr<const QuadTreeNode> SouthChildNode) const;

	TSharedPtr<const QuadTreeNode> ConstructCenteredGrandchild() const;


};
