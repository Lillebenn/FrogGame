// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AdvCreature.h"
#include "FrogChild.generated.h"

/**
 * 
 */
UCLASS()
class FROGGAME_API AFrogChild : public AAdvCreature
{
	GENERATED_BODY()

	UPROPERTY (VisibleAnywhere, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* DirectionArrow;
public:
	AFrogChild();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsCollected{false};

	void MoveToSwamp();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Frog Child")
	FTransform SwampLocation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Sound)
	USoundCue* FrogScreech;
};
