// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EdibleInfo.h"
#include "BaseEdible.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FBaseEdible
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Edible)
	FEdibleInfo EdibleInfo;
};
