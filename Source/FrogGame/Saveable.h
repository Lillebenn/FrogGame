// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Saveable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USaveable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class FROGGAME_API ISaveable
{
	GENERATED_BODY()

public:
	// Functions that can be used for implementing custom behaviour when saving and loading
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Saveable Actor")
	void ActorSaveDataLoaded();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Saveable Actor")
	void ActorSaveDataSaved();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Saveable Actor")
	FTransform GetStartTransform();
};
