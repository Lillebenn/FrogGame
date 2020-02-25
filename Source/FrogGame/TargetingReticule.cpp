// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetingReticule.h"
#include "UObject/ConstructorHelpers.h"


UTargetingReticule::UTargetingReticule()
{
	SetWidgetSpace(EWidgetSpace::Screen);
	SetAbsolute(false, true, true);
	 static ConstructorHelpers::FClassFinder<UUserWidget> ReticuleClass(TEXT("/Game/Blueprints/HUD/BP_Reticule"));
	if(ReticuleClass.Class != nullptr)
	{
		SetWidgetClass(ReticuleClass.Class);
	}
}

void UTargetingReticule::HideReticule()
{
	SetHiddenInGame(true);
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
}

void UTargetingReticule::DrawReticule(FVector Position, float LifeTime)
{
	// Set the timer to re-hide the Reticule if we stop receiving calls to this function.
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UTargetingReticule::HideReticule, LifeTime, false);
	SetHiddenInGame(false);
}

void UTargetingReticule::BeginPlay()
{
	SetHiddenInGame(true);
}
