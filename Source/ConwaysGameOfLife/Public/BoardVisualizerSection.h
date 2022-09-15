// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BoardUtilities.h"
#include "BoardVisualizerSection.generated.h"

class UGameBoard;

UCLASS(Blueprintable, meta=(BlueprintSpawnableComponent))
class CONWAYSGAMEOFLIFE_API ABoardVisualizerSection : public AActor
{
	GENERATED_BODY()
	
public:
	// The dimension of the section of this board that this Actor represents.
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta=(DisplayName="Section Dimension"))
	int mSectionDimension;

	// The Actor class we should use to represent cells.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "Cell Representation Actor Class"))
	TSubclassOf<AActor> mCellRepresentationActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "X Coordinate To Represent"))
	int64 mXCoordinateToRepresent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "Y Coordinate To Represent"))
	int64 mYCoordinateToRepresent;

	// Sets default values for this actor's properties
	ABoardVisualizerSection();

	UPROPERTY(BlueprintReadWrite, meta = (DisplayName = "Coordinate To Cell Actor Map"))
	TMap<FBoardCoordinate, AActor*> mCoordinateToCellActorMap;

	// Event to initialize the board.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void InitializeWithSectionDimension(int SectionDimension);

	void InitializeWithSectionDimension_Implementation(int SectionDimension);

	// Update the visualizer section to the current state of the provided game board.
	UFUNCTION(BlueprintCallable)
	void UpdateRepresentation(const UGameBoard* GameBoard);
};
