// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DestructibleActor.h"
#include "EdibleInfo.h"
#include "BaseEdible.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class FROGGAME_API ABaseEdible : public ADestructibleActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Edible)
	FEdibleInfo EdibleInfo;

};
