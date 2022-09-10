// Fill out your copyright notice in the Description page of Project Settings.


#include "QuadTreeLeaf.h"

// Basic constructor. A leaf's level will always be 0.
QuadTreeLeaf::QuadTreeLeaf(const bool isAlive) :
	QuadTreeNode(0),
	mIsAlive(isAlive)
{

}

bool QuadTreeLeaf::GetBit(const int64 X, const int64 Y) const
{
	return mIsAlive;
}

TSharedPtr<const QuadTreeNode> QuadTreeLeaf::SetBit(const int64 X, const int64 Y) const
{
	if (X != 0 || Y != 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Some coordinate at the leaf level was nonzero. This should never happen!"));
	}

	// TODO: Override with our custom creation when doing canonization.
	return MakeShareable<QuadTreeNode>(new QuadTreeLeaf(true));
}


ChildNode QuadTreeLeaf::GetChildAndLocalCoordinates(const int64 X, const int64 Y, int64& LocalXOut, int64& LocalYOut) const
{
	UE_LOG(LogTemp, Error, TEXT("We should never be calling this function on a leaf!"));

	return ChildNode::Northeast;
}
