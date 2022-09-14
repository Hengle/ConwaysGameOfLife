// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BoardVisualizerSection.generated.h"


UCLASS(Blueprintable, meta=(BlueprintSpawnableComponent))
class CONWAYSGAMEOFLIFE_API ABoardVisualizerSection : public AActor
{
	GENERATED_BODY()
	
public:	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(DisplayName="Section Dimension"))
	int mSectionDimension;

	// Sets default values for this actor's properties
	ABoardVisualizerSection();

	//void SetRepresentedTreeNode(const QuadTreeNode* RepresentedNode);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "Live Cell Representation Actor Class"))
	TSubclassOf<AActor> mLiveCellRepresentationActorClass;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


private:
	int64 mGlobalXOfTopLeftCell;
	
	int64 mGlobalYOfTopLeftCell;

	

	// We represent an entire node of the board, with size mSectionDimension.
	//TSharedPtr<const QuadTreeNode> mRepresentedTreeNode;
};
