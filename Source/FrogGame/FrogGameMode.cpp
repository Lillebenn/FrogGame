// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "FrogGameMode.h"
#include "FrogGameCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "FrogGameInstance.h"

AFrogGameMode::AFrogGameMode()
{
}


void AFrogGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (FrogHUDClass != nullptr)
	{
		CurrentWidget = CreateWidget<UFrogGameUI>(GetWorld(), FrogHUDClass);

		if (CurrentWidget != nullptr && bShowWidget)
		{
			CurrentWidget->AddToViewport();
		}
	}
}

void AFrogGameMode::SetWidgetVisibility()
{
	if (CurrentWidget)
	{
		ESlateVisibility CurrentVisibility{ESlateVisibility::Hidden};
		if (CurrentWidget->GetVisibility() == ESlateVisibility::Hidden)
		{
			CurrentVisibility = ESlateVisibility::Visible;
		}
		CurrentWidget->SetVisibility(CurrentVisibility);
	}
}

UFrogGameUI* AFrogGameMode::GetGameHUD() const
{
	return CurrentWidget;
}