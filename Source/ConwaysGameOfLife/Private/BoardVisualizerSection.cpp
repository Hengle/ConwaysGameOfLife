// Fill out your copyright notice in the Description page of Project Settings.


#include "BoardVisualizerSection.h"

// Sets default values
ABoardVisualizerSection::ABoardVisualizerSection()
{
 	// This Actor should not tick, it should be told when to update via the Controller.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ABoardVisualizerSection::BeginPlay()
{
	Super::BeginPlay();
	
}

/*
void ABoardVisualizerSection::SetRepresentedTreeNode(const QuadTreeNode* RepresentedNode)
{
	mRepresentedTreeNode = MakeShareable<const QuadTreeNode>(RepresentedNode);
}
*/


