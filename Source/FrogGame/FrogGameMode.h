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


public:
	AFrogGameMode();

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "User Interface", Meta = (BlueprintProtected = "true"))
	TSubclassOf<class UUserWidget> FrogHUDClass;

	UPROPERTY()
	class UUserWidget* CurrentWidget;

	void BeginPlay() override;
};
