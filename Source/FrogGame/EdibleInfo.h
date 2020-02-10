// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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
	float GrowthCoefficient{0.5f};
	// Ammount of Points earned towards getting powermode gained when eating this object.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Edible)
	float PowerPoints{ 1.f };
	// Amount of Score granted for eating this edible.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Edible)
	float ScorePoints{0.1f};
	// Size tier of the object. Clamped to 10.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Edible, meta = (ClampMax = 10))
	uint8 SizeTier{0};
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Edible)
	float Size;

};
