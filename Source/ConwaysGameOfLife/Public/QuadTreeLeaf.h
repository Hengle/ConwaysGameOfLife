// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "QuadTreeNode.h"

/**
 * 
 */
class CONWAYSGAMEOFLIFE_API QuadTreeLeaf : QuadTreeNode
{
public:
	QuadTreeLeaf(const bool isAlive);

	bool GetBit(const int64 X, const int64 Y) const override;
	const TSharedPtr<const QuadTreeNode> SetBit(const int64 X, const int64 Y) const override;
private:
	const bool mIsAlive;

	ChildNode GetChildAndLocalCoordinates(const int64 X, const int64 Y, int64& LocalXOut, int64& LocalYOut) const override;
};
