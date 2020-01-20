// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DestructibleActor.h"
#include "BaseEdible.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class FROGGAME_API ABaseEdible : public ADestructibleActor
{
	GENERATED_BODY()

public:

	/**
	 * Returns the size of the destructible mesh's bounding box.
	 * Don't use for detail-sensitive tasks. Very approximate.
	 */
	float GetFullSize() const;
	// Actual growth increase from eating this object is retrieved by multiplying by object size with this coefficient.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Edible)
	float GrowthCoefficient{1.f};
	// Size tier of the object. Clamped to 10.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Edible, meta = (ClampMax = 10))
	uint8 SizeTier{1};

	// TODO: Figure out how to select/interact with individual mesh chunks.
	void Consume(float FrogSize, const FString& BoneName);
};
