// Fill out your copyright notice in the Description page of Project Settings.


#include "BoardVisualizerSection.h"

// Sets default values
ABoardVisualizerSection::ABoardVisualizerSection()
{
 	// This Actor should not tick, it should be told when to update via the Controller.
	PrimaryActorTick.bCanEverTick = false;
}


void ABoardVisualizerSection::InitializeWithSectionDimension_Implementation(int SectionDimension)
{
	mSectionDimension = SectionDimension;
}

void ABoardVisualizerSection::UpdateRepresentation_Implementation(const TArray<FBoardCoordinate>& LiveCoordinates)
{

}
