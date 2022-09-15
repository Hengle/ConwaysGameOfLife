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

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int64 xCoordToRepresent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int64 yCoordToRepresent;

	uint64 mXCoordinateToRepresent;

	uint64 mYCoordinateToRepresent;

	// Sets default values for this actor's properties
	ABoardVisualizerSection();

	UPROPERTY(BlueprintReadWrite, meta = (DisplayName = "Coordinate To Cell Actor Map"))
	TMap<FBoardCoordinate, AActor*> mCoordinateToCellActorMap;

	// Event to initialize the board.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void InitializeWithSectionDimension(int SectionDimension);

	void InitializeWithSectionDimension_Implementation(int SectionDimension);

	UFUNCTION(BlueprintCallable)
	void SetCoordinateToRepresent(FBoardCoordinate Coordinate);

	// Update the visualizer section to the current state of the provided game board.
	UFUNCTION(BlueprintCallable)
	void UpdateRepresentation(const UGameBoard* GameBoard);

	UFUNCTION(BlueprintCallable)
	void AddCellToMap(int64 LocalXCoordinate, int64 LocalYCoordinate, AActor* Cell);
};
