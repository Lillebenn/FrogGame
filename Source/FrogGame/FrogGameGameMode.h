// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "FrogGameGameMode.generated.h"

UCLASS(minimalapi)
class AFrogGameGameMode : public AGameModeBase
{
	GENERATED_BODY()

		virtual void BeginPlay() override; // Override beginplay from the baseclass.

public:
	AFrogGameGameMode();

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Score", Meta = (BlueprintProtected = "true"))
	TSubclassOf<class UUserWidget> FrogGameHUDClass;

	UPROPERTY()
	class UUserWidget* CurrentWidget;
};

	

