// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "TimerManager.h"
#include "TargetingReticule.generated.h"

/**
 * 
 */
UCLASS()
class FROGGAME_API UTargetingReticule : public UWidgetComponent
{
	GENERATED_BODY()


public:
	UTargetingReticule();
	FTimerDelegate TimerDel;
	FTimerHandle TimerHandle;

	UFUNCTION()
	void HideReticule();
	void DrawReticule(FVector Position, float LifeTime);
protected:
	void BeginPlay() override;
};
