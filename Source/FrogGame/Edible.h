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
	 * Returns the Edible Info of the object.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = Gameplay)
	FEdibleInfo GetInfo() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = Gameplay)
	void DisableActor();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = Gameplay)
	void OnDisabled();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = Gameplay)
	USceneComponent* GetTargetComponent();

};
