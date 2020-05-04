// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FrogGameUI.h"
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

	void SetWidgetVisibility();

	void ShowAllFrogsGathered() const
	{
		Cast<UFrogGameUI>(CurrentWidget)->AllFrogsGathered();
	}

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "User Interface", Meta = (BlueprintProtected = "true"))
	TSubclassOf<class UUserWidget> FrogHUDClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "User Interface")
	bool bShowWidget{true};
	UPROPERTY()
	class UUserWidget* CurrentWidget;


	void BeginPlay() override;
};
