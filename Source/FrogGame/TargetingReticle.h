// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "TimerManager.h"
#include "TargetingReticle.generated.h"

/**
 * 
 */
UCLASS()
class FROGGAME_API UTargetingReticle : public UWidgetComponent
{
	GENERATED_BODY()


public:
	UTargetingReticle();
	FTimerDelegate TimerDel;
	FTimerHandle TimerHandle;

	UFUNCTION()
	void HideReticle();
	void DrawReticle(FVector Position, float LifeTime);
protected:
	void BeginPlay() override;
};
