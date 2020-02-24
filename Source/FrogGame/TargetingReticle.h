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

	// Distance between the reticle's parent component and the reticle. 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AutoAim)
	float ReticleDistance{0.f};
	UFUNCTION()
	void HideReticle();
	void DrawReticle(const FVector Position, const FRotator Orientation, float LifeTime);
protected:
	void BeginPlay() override;
};
