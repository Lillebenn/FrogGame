// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "FrogGameMode.h"
#include "FrogGameCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

AFrogGameMode::AFrogGameMode()
{
	// set default pawn class to our Blueprinted character
	if (CharacterBP != nullptr)
	{
		DefaultPawnClass = CharacterBP;
	}
}

void AFrogGameMode::BeginPlay()
{
	Super::BeginPlay();

	AFrogGameCharacter* MyCharacter = Cast<AFrogGameCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));

	if (FrogGameHUDClass != nullptr)
	{
		CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), FrogGameHUDClass);

			if (CurrentWidget != nullptr)
			{
				CurrentWidget->AddToViewport();
			}
	}
}


