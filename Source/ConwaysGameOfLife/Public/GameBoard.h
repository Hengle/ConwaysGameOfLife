// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "QuadTreeNode.h"

#include "GameBoard.generated.h"



constexpr int sBoardSize = 8;
/**
 * 
 */
UCLASS(BlueprintType, meta=(BlueprintSpawnableComponent))
class CONWAYSGAMEOFLIFE_API UGameBoard : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void CreateTree();

	UFUNCTION(BlueprintCallable)
	void SetBit(const int64 X, const int64 Y);

	UFUNCTION(BlueprintCallable)
	FString GetBoardString() const;
	
private:
	TSharedPtr<const QuadTreeNode> mRootNode;
};

