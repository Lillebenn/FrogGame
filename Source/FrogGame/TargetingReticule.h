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
	FTimerHandle TimerHandle;

	UPROPERTY()
	class UImage* Reticule;
	UPROPERTY()
	class UCanvasPanelSlot* ReticuleSlot;
	UFUNCTION()
	void HideReticule();
	void DrawReticule(FVector Position, float LifeTime, float Size = 70.f, float Alpha = 1.f);
protected:
	void BeginPlay() override;

private:
	bool bUpdatedSlot{false};
};
