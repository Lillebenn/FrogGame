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
		CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), FrogHUDClass);

		if (CurrentWidget != nullptr)
		{
			CurrentWidget->AddToViewport();
		}
	}
	Cast<UFrogGameInstance>(GetGameInstance())->CreateCheckpoint();
	if(bNewLevel)
	{
		UFrogGameInstance* Instance{Cast<UFrogGameInstance>(GetGameInstance())};
		if(Instance)
		{
			Instance->LoadCurrentSave();
			bNewLevel = false;
		}
	}
}