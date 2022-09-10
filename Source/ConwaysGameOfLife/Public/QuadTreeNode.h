// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

enum ChildNode
{
	Northwest = 0,
	Northeast = 1,
	Southwest = 2,
	Southeast = 3
};

/**
 * 
 */
class CONWAYSGAMEOFLIFE_API QuadTreeNode
{
public: 
	static TSharedPtr<const QuadTreeNode> CreateEmptyNode(const uint8 NumLevels);

	static TSharedPtr<const QuadTreeNode> CreateNodeWithSubnodes(const uint8 Level, const TSharedPtr<const QuadTreeNode> Northwest, const TSharedPtr<const QuadTreeNode> Northeast, const TSharedPtr<const QuadTreeNode> Southwest, const TSharedPtr<const QuadTreeNode> Southeast);

	static TSharedPtr<const QuadTreeNode> GetNextGenerationOfCenterCellInBitmask;


public:
	const uint8 mLevel;

	QuadTreeNode(const uint8 Level);
	QuadTreeNode(const uint8 Level, const TSharedPtr<const QuadTreeNode> Northwest, const TSharedPtr<const QuadTreeNode> Northeast, const TSharedPtr<const QuadTreeNode> Southwest, const TSharedPtr<const QuadTreeNode> Southeast);

	// Returns the value of the bit at X and Y, where X and Y are local coordinates in this block.
	virtual bool GetBit(const int64 X, const int64 Y) const;

	// Returns a node that is the same as the current node, but with the bit at X and Y set to alive.
	virtual TSharedPtr<const QuadTreeNode> SetBit(const int64 X, const int64 Y) const;

	TSharedPtr<const QuadTreeNode> GetChild(ChildNode Node) const;

	// Returns a node representing how the centered (mLevel - 1) node would look if advanced one generation.
	virtual TSharedPtr<const QuadTreeNode> GetNextGeneration() const;
private:
	// Pointers to each of our children, which each represent 1/4 of our space on the board.
	TStaticArray<TSharedPtr<const QuadTreeNode>, 4> mChildren;

	// Returns the child node that X and Y are contained in. Puts the relative coordinates for X and Y within that child in the out params.
	virtual ChildNode GetChildAndLocalCoordinates(const int64 X, const int64 Y, int64& LocalXOut, int64& LocalYOut) const;
};
