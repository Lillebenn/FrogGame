// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "FrogGameGameMode.h"
#include "FrogGameCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

void AFrogGameGameMode::BeginPlay()
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

AFrogGameGameMode::AFrogGameGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
