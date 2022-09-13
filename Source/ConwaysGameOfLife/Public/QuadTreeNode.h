// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

enum ChildNode : int8
{
	Northwest = 0,
	Northeast = 1,
	Southwest = 2,
	Southeast = 3,

	kCount = 4
};

/**
 * 
 */
class CONWAYSGAMEOFLIFE_API QuadTreeNode
{
public:
	static TSharedPtr<const QuadTreeNode> CreateLeaf(bool IsAlive);

	static TSharedPtr<const QuadTreeNode> CreateNodeWithSubnodes(const uint8 Level, const TSharedPtr<const QuadTreeNode> Northwest, const TSharedPtr<const QuadTreeNode> Northeast, const TSharedPtr<const QuadTreeNode> Southwest, const TSharedPtr<const QuadTreeNode> Southeast);
	
	static TSharedPtr<const QuadTreeNode> CreateEmptyNode(const uint8 NumLevels);

	static TSharedPtr<const QuadTreeNode> GetNextGenerationCellFromNeighborhood(uint16 NeighborhoodBitset);

public:
	const uint8 mLevel;

	QuadTreeNode(const bool IsAlive);
	QuadTreeNode(const uint8 Level, const TSharedPtr<const QuadTreeNode> Northwest, const TSharedPtr<const QuadTreeNode> Northeast, const TSharedPtr<const QuadTreeNode> Southwest, const TSharedPtr<const QuadTreeNode> Southeast);

	// Returns the status of the cell at X and Y, where X and Y are local coordinates in this block.
	bool GetIsCellAlive(const int64 X, const int64 Y) const;

	// Returns a node that is the same as the current node, but with the bit at X and Y set to alive.
	TSharedPtr<const QuadTreeNode> SetCellToAlive(const int64 X, const int64 Y) const;

	TSharedPtr<const QuadTreeNode> GetChild(ChildNode Node) const;

	// Returns a node representing how a centered mLevel-1 size node would look if advanced one generation.
	TSharedPtr<const QuadTreeNode> GetNextGeneration() const;

	TSharedPtr<const QuadTreeNode> ConstructCenteredChild() const;

	uint64 GetBlockDimension() const;

	FString GetNodeString() const;

	TSharedPtr<const QuadTreeNode> Northwest() const;

	TSharedPtr<const QuadTreeNode> Northeast() const;

	TSharedPtr<const QuadTreeNode> Southwest() const;

	TSharedPtr<const QuadTreeNode> Southeast() const;

	bool IsLeaf() const;

	bool IsAlive() const;

	TSharedPtr<const QuadTreeNode> GetBlockOfDimensionContainingCoordinate(const uint64 DesiredDimension, const int64 X, const int64 Y) const;

private:
	// Pointers to each of our children, which each represent 1/4 of this node's space on the board.
	TStaticArray<TSharedPtr<const QuadTreeNode>, 4> mChildren;

	bool mIsAlive;

	// Returns the child node that X and Y are contained in. Puts the relative coordinates for X and Y within that child in the out params.
	ChildNode GetChildAndLocalCoordinates(const int64 X, const int64 Y, int64& LocalXOut, int64& LocalYOut) const;

	// Returns a node representing the centered 2x2 interior square of cells if they were advanced one generation.
	TSharedPtr<const QuadTreeNode> Run4x4Simulation() const;

	TSharedPtr<const QuadTreeNode> ConstructHorizontalCenteredGrandchild(TSharedPtr<const QuadTreeNode> WestChildNode, TSharedPtr<const QuadTreeNode> EastChildNode) const;
	
	TSharedPtr<const QuadTreeNode> ConstructVerticalCenteredGrandchild(TSharedPtr<const QuadTreeNode> NorthChildNode, TSharedPtr<const QuadTreeNode> SouthChildNode) const;

	TSharedPtr<const QuadTreeNode> ConstructCenteredGrandchild() const;
};
