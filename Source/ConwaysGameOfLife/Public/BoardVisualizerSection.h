// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BoardUtilities.h"
#include "BoardVisualizerSection.generated.h"

UCLASS(Blueprintable, meta=(BlueprintSpawnableComponent))
class CONWAYSGAMEOFLIFE_API ABoardVisualizerSection : public AActor
{
	GENERATED_BODY()
	
public:
	// The dimension of the section of this board that this Actor represents.
	UPROPERTY(BlueprintReadOnly, meta=(DisplayName="Section Dimension"))
	int mSectionDimension;

	// The Actor class we should use to represent cells.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "Cell Representation Actor Class"))
	TSubclassOf<AActor> mCellRepresentationActorClass;

	// Sets default values for this actor's properties
	ABoardVisualizerSection();

	// Event to initialize the board.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void InitializeWithSectionDimension(int SectionDimension);

	void InitializeWithSectionDimension_Implementation(int SectionDimension);

	// Event to update the representation with a new list of live coordinates.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void UpdateRepresentation(const TArray<FBoardCoordinate>& LiveCoordinates);

	void UpdateRepresentation_Implementation(const TArray<FBoardCoordinate>& LiveCoordinates);
};
