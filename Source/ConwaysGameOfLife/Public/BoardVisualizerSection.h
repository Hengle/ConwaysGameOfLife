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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "Live Cell Representation Actor Class"))
	TSubclassOf<AActor> mLiveCellRepresentationActorClass;

	// Sets default values for this actor's properties
	ABoardVisualizerSection();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
