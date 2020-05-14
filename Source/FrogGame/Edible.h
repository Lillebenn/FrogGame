// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EdibleComponent.h"
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
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool IsDisabled();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnDisabled();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void DisableActor();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void PauseAI(bool bPause = true);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetActive(bool bActive);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool IsActive();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetMobility(bool bShouldActivate);
};
