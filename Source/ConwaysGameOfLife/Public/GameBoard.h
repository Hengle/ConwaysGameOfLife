// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "QuadTreeNode.h"

#include "GameBoard.generated.h"


/**
 * 
 */
UCLASS(BlueprintType, meta=(BlueprintSpawnableComponent))
class CONWAYSGAMEOFLIFE_API UGameBoard : public UObject
{
	GENERATED_BODY()

public:
	// TODO add error handling for trying to construct a new board when one already exists?
	UFUNCTION(BlueprintCallable)
	void ConstructBoard(int BoardDimension);

	UFUNCTION(BlueprintCallable)
	void ConstructMaxSizeBoard();

	UFUNCTION(BlueprintCallable)
	void SetBit(const int64 X, const int64 Y);

	UFUNCTION(BlueprintCallable)
	FString GetBoardString() const;

	UFUNCTION(BlueprintCallable)
	void SimulateNextGeneration();
	
private:
	uint64 mBoardDimension;

	uint8 mMaxLevelInTree;

	TSharedPtr<const QuadTreeNode> mRootNode;

	TSharedPtr<const QuadTreeNode> ConstructBoardWithCenteredQuadrant(ChildNode QuadrantToCenter) const;

	void ConstructBoardHelper(uint64 BoardDimension);
};

