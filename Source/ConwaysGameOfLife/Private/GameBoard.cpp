// Fill out your copyright notice in the Description page of Project Settings.


#include "GameBoard.h"

void UGameBoard::CreateTree()
{
	// TEMP
	const int startingLevel = 2;

	mRootNode = MakeShareable<QuadTreeNode>(new QuadTreeNode(startingLevel));
}

void UGameBoard::SetBit(const int64 X, const int64 Y)
{
	mRootNode = mRootNode->SetBit(X, Y);
}