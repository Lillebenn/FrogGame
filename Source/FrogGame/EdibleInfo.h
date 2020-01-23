// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Class.h"
#include "EdibleInfo.generated.h"

/**
 * Struct for holding information about an edible object. 
 */
USTRUCT(BlueprintType)
struct FEdibleInfo
{
    GENERATED_BODY()

	// Actual growth increase from eating this object is retrieved by multiplying by object size with this coefficient.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Edible)
	float GrowthCoefficient{1.f};
	// Size tier of the object. Clamped to 10.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Edible, meta = (ClampMax = 10))
	uint8 SizeTier{1};
	// Typically holds the bounding size of an object's collider. Probably not a very accurate representation of size.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Edible)
	FVector RoughSize;
};
