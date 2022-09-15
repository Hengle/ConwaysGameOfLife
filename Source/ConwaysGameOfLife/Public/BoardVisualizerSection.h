// Conway's Game Of Life in Unreal
// Ilana Franklin, 2022
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BoardUtilities.h"
#include "BoardVisualizerSection.generated.h"

class UGameBoard;

/**
 * An Actor responsible for visualizing one block on a GameBoard.
 */
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

	// The X component of the signed coordinate in the board that this section should represent.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "X Coordinate to Represent"))
	int64 mSignedXCoordToRepresent;

	// The Y component of the signed coordinate in the board that this section should represent.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "Y Coordinate to Represent"))
	int64 mSignedYCoordToRepresent;

	// The X component of the unsigned coordinate in the board that this section should represent.
	uint64 mXCoordinateToRepresent;

	// The Y component of the unsigned coordinate in the board that this section should represent.
	uint64 mYCoordinateToRepresent;

	// Sets default values for this actor's properties
	ABoardVisualizerSection();

	// A map of local FBoardCoordinate to their corresponding Cell Actors in the visualization.
	UPROPERTY(BlueprintReadWrite, meta = (DisplayName = "Coordinate To Cell Actor Map"))
	TMap<FBoardCoordinate, AActor*> mCoordinateToCellActorMap;

	// Sets the coordinate that this block represents to Coordinate.
	UFUNCTION(BlueprintCallable)
	void SetCoordinateToRepresent(FBoardCoordinate Coordinate);

	// Update the visualizer section to the current state of the provided game board.
	UFUNCTION(BlueprintCallable)
	void UpdateRepresentation(const UGameBoard* GameBoard);

	// Converts local coordinates to their unsigned equivalents and adds them to the Coordinate->Cell Actor map.
	UFUNCTION(BlueprintCallable)
	void AddCellToMap(int64 LocalXCoordinate, int64 LocalYCoordinate, AActor* Cell);
};
