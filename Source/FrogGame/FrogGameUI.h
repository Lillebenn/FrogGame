// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FrogGameUI.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class FROGGAME_API UFrogGameUI : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintImplementableEvent)
	void PlayInAnimation();
	UFUNCTION(BlueprintImplementableEvent)
	void PlayOutAnimation();
	UFUNCTION(BlueprintImplementableEvent)
	void AllFrogsGathered();
	UFUNCTION(BlueprintImplementableEvent)
	void DamageTaken(float Damage);
	UFUNCTION(BlueprintImplementableEvent)
	void CanEnterPowerMode();
	UFUNCTION(BlueprintImplementableEvent)
	void EnteredPowerMode();
	UFUNCTION(BlueprintImplementableEvent)
	void EnteredRegularMode();
	UFUNCTION(BlueprintImplementableEvent)
	void GainedPowerPoints(bool bInPowerMode);
	UFUNCTION(BlueprintImplementableEvent)
	void GainedScorePoints();
	UFUNCTION(BlueprintImplementableEvent)
	void GainedDestruction();
	UFUNCTION(BlueprintImplementableEvent)
	void GainedFrogChild();
};
