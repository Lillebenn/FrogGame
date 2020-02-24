// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetingReticle.h"




void UTargetingReticle::HideReticle()
{
	SetHiddenInGame(true);
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
}

void UTargetingReticle::DrawReticle(const FVector Position, const FRotator Orientation, float LifeTime)
{
	// Set the timer to re-hide the reticle if we stop receiving calls to this function.
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UTargetingReticle::HideReticle, LifeTime, false);
	UE_LOG(LogTemp, Warning, TEXT("Drawing Reticle!"))
	SetHiddenInGame(false);
	//SetRelativeLocationAndRotation(Position, Orientation);
}

void UTargetingReticle::BeginPlay()
{
	ReticleDistance = GetRelativeLocation().Size();
	//SetHiddenInGame(true);
}
