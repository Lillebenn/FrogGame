// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EdibleInfo.h"
#include "Edible.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UEdible : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class FROGGAME_API IEdible
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/**
	 * Returns the size of the destructible mesh's bounding box.
	 * Don't use for detail-sensitive tasks. Very approximate.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = Gameplay)
	FEdibleInfo GetInfo() const;
};
