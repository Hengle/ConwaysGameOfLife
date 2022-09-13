// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BoardVisualizerSection.generated.h"


UCLASS(Blueprintable)
class CONWAYSGAMEOFLIFE_API ABoardVisualizerSection : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABoardVisualizerSection();

	//void SetRepresentedTreeNode(const QuadTreeNode* RepresentedNode);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


private:
	int64 mGlobalXOfTopLeftCell;
	
	int64 mGlobalYOfTopLeftCell;

	UPROPERTY(EditDefaultsOnly)
	int mSectionDimension;

	// We represent an entire node of the board, with size mSectionDimension.
	//TSharedPtr<const QuadTreeNode> mRepresentedTreeNode;
};
