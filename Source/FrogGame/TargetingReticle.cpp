// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetingReticle.h"

UTargetingReticle::UTargetingReticle()
{

	SetHiddenInGame(true);
}



void UTargetingReticle::HideReticle()
{
	SetHiddenInGame(true);
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
}

void UTargetingReticle::DrawReticle(const FVector Position, const FRotator Orientation, float LifeTime)
{
	// Set the timer to re-hide the reticle if we stop receiving calls to this function.
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UTargetingReticle::HideReticle, 1.f, false);

	SetHiddenInGame(false);
	SetWorldLocationAndRotation(Position * ReticleDistance, Orientation);
}

void UTargetingReticle::BeginPlay()
{
	ReticleDistance = GetRelativeLocation().Size();
}
