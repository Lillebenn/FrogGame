// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "FrogGameMode.generated.h"

UCLASS(minimalapi)
class AFrogGameMode : public AGameModeBase
{
	GENERATED_BODY()

		virtual void BeginPlay() override; // Override beginplay from the baseclass.

public:
	AFrogGameMode();

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Score", Meta = (BlueprintProtected = "true"))
	TSubclassOf<class UUserWidget> FrogGameHUDClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AFrogGameCharacter> CharacterBP;
	UPROPERTY()
	class UUserWidget* CurrentWidget;
};

	

