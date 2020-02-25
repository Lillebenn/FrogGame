// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetingReticle.h"


UTargetingReticle::UTargetingReticle()
{
	SetWidgetSpace(EWidgetSpace::Screen);
	SetAbsolute(false, true, true);

}

void UTargetingReticle::HideReticle()
{
	SetHiddenInGame(true);
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
}

void UTargetingReticle::DrawReticle(FVector Position, float LifeTime)
{
	// Set the timer to re-hide the reticle if we stop receiving calls to this function.
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UTargetingReticle::HideReticle, LifeTime, false);
	SetHiddenInGame(false);
}

void UTargetingReticle::BeginPlay()
{
	SetHiddenInGame(true);
}
