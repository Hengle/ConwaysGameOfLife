// Conway's Game Of Life in Unreal
// Ilana Franklin, 2022

#pragma once

#include "CoreMinimal.h"

// The different quadrants/children that are present in one QuadTreeNode.
enum ChildNode : int8
{
	Northwest = 0,
	Northeast = 1,
	Southwest = 2,
	Southeast = 3,

	kCount = 4
};

/**
 * A class representing one node of a QuadTree that contains data for the Game of Life board.
 */
class CONWAYSGAMEOFLIFE_API QuadTreeNode
{
public:
	// Create a node full of dead cells starting at level = NumLevels;
	static TSharedPtr<const QuadTreeNode> CreateEmptyNode(const uint8 NumLevels);
	
	// Create a node at Level with the four provided nodes as children.
	static TSharedPtr<const QuadTreeNode> CreateNodeWithSubnodes(const uint8 Level, const TSharedPtr<const QuadTreeNode> Northwest, const TSharedPtr<const QuadTreeNode> Northeast, const TSharedPtr<const QuadTreeNode> Southwest, const TSharedPtr<const QuadTreeNode> Southeast);

	// Create a leaf. 
	static TSharedPtr<const QuadTreeNode> CreateLeaf(bool IsAlive);

private:
	// The canonical live cell. We have only one of these in order to cut down on memory requirements.
	static TSharedPtr<const QuadTreeNode> sCanonicalLiveCell;

	// The canonical dead cell. We have only one of these in order to cut down on memory requirements.
	static TSharedPtr<const QuadTreeNode> sCanonicalDeadCell;

	// Given a bitset representing a neighborhood (block of nine cells), return a leaf representing the center cell in the next generation.
	static TSharedPtr<const QuadTreeNode> GetNextGenerationCellFromNeighborhood(uint16 NeighborhoodBitset);

public:
	// The level of this node in the tree.
	const uint8 mLevel;

	// Basic constructor for a node. Returns a leaf.
	QuadTreeNode(const bool IsAlive);
	
	// Basic constructor for a node. Returns a node at Level with the four provided nodes as children.
	QuadTreeNode(const uint8 Level, const TSharedPtr<const QuadTreeNode> Northwest, const TSharedPtr<const QuadTreeNode> Northeast, const TSharedPtr<const QuadTreeNode> Southwest, const TSharedPtr<const QuadTreeNode> Southeast);

	// Basic == operator.
	bool operator==(const QuadTreeNode& Other) const;

	// Returns the status of the cell at X and Y, where X and Y are local coordinates in this block.
	bool GetIsCellAlive(const int64 X, const int64 Y) const;

	// Returns a node that is the same as the current node, but with the bit at X and Y set to alive.
	TSharedPtr<const QuadTreeNode> SetCellToAlive(const int64 X, const int64 Y) const;

	// Returns the child node corresponding to Node.
	TSharedPtr<const QuadTreeNode> GetChild(ChildNode Node) const;

	// Returns a node representing how a centered GetNodeDimension()xGetNodeDimension() portion of this node would look if advanced one generation.
	TSharedPtr<const QuadTreeNode> GetNextGeneration() const;

	// Constructs a node at mLevel - 1 using the cells at the center of this node.
	TSharedPtr<const QuadTreeNode> ConstructCenteredChild() const;

	// Returns the dimension of this node. Each node represents a NodeDimensionxNodeDimension portion of the entire board.
	uint64 GetNodeDimension() const;

	// Returns a string representing how this node looks. For Debug purposes.
	FString GetNodeString() const;

	// Returns the child representing the Northwest quadrant of this node.
	TSharedPtr<const QuadTreeNode> Northwest() const;

	// Returns the child representing the Northeast quadrant of this node.
	TSharedPtr<const QuadTreeNode> Northeast() const;

	// Returns the child representing the Southwest quadrant of this node.
	TSharedPtr<const QuadTreeNode> Southwest() const;

	// Returns the child representing the Southeast quadrant of this node.
	TSharedPtr<const QuadTreeNode> Southeast() const;

	// Returns whether or not this node is a leaf.
	bool IsLeaf() const;

	// Returns whether or not this node is alive, i.e. whether or not it contains any live cells.
	bool IsAlive() const;

	// Returns the node with size DesiredDimensionxDesiredDimension that contains the cell with coordinates (X, Y).
	TSharedPtr<const QuadTreeNode> GetBlockOfDimensionContainingCoordinate(const uint64 DesiredDimension, const int64 X, const int64 Y) const;

private:
	// Pointers to each of our children, which each represent 1/4 of this node's space on the board.
	TStaticArray<TSharedPtr<const QuadTreeNode>, 4> mChildren;

	// Indicates whether or not this node contains any live cells.
	bool mIsAlive;

	// Returns the child node that X and Y are contained in. Puts the relative coordinates for X and Y within that child in the out params.
	ChildNode GetChildAndLocalCoordinates(const int64 X, const int64 Y, int64& LocalXOut, int64& LocalYOut) const;

	// Returns a node representing the centered 2x2 interior square of cells if they were advanced one generation.
	TSharedPtr<const QuadTreeNode> Run4x4Simulation() const;

	// Constructs a node at level mLevel - 2 that is centered horizontally between the two provided quadrants.
	TSharedPtr<const QuadTreeNode> ConstructHorizontalCenteredGrandchild(TSharedPtr<const QuadTreeNode> WestChildNode, TSharedPtr<const QuadTreeNode> EastChildNode) const;
	
	// Constructs a node at level mLevel - 2 that is centered vertically between the two provided quadrants.
	TSharedPtr<const QuadTreeNode> ConstructVerticalCenteredGrandchild(TSharedPtr<const QuadTreeNode> NorthChildNode, TSharedPtr<const QuadTreeNode> SouthChildNode) const;

	// Constructs a node at level mLevel - 2 that is centered between all four quadrants of this node.
	TSharedPtr<const QuadTreeNode> ConstructCenteredGrandchild() const;
};
