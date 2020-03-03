// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetingReticule.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/Image.h"
#include "Components/CanvasPanelSlot.h"


UTargetingReticule::UTargetingReticule()
{
	SetWidgetSpace(EWidgetSpace::Screen);
	SetAbsolute(false, true, true);
	static ConstructorHelpers::FClassFinder<UUserWidget> ReticuleClass(TEXT("/Game/Blueprints/HUD/BP_Reticule"));
	if (ReticuleClass.Class != nullptr)
	{
		SetWidgetClass(ReticuleClass.Class);
	}else
	{
		UE_LOG(LogTemp, Warning, TEXT("Missing BP_Reticule reference!"));
	}
}

void UTargetingReticule::HideReticule()
{
	SetHiddenInGame(true);
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
	bUpdatedSlot = false;
}

void UTargetingReticule::DrawReticule(FVector Position, float LifeTime, float Size)
{
	// Set the timer to re-hide the Reticule if we stop receiving calls to this function.
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UTargetingReticule::HideReticule, LifeTime, false);
	SetHiddenInGame(false);
	if (!bUpdatedSlot)
	{
		if (ReticuleSlot)
		{
			ReticuleSlot->SetSize(FVector2D(Size));
			bUpdatedSlot = true;
		}
	}
}

void UTargetingReticule::BeginPlay()
{
	Super::BeginPlay();
	SetHiddenInGame(true);

	Reticule = Cast<UImage>(GetUserWidgetObject()->GetWidgetFromName(TEXT("Reticule")));
	if (Reticule)
	{
		ReticuleSlot = Cast<UCanvasPanelSlot>(Reticule->Slot);
	}
}
