// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FrogGameUI.h"
#include "GameFramework/GameModeBase.h"

#include "Blueprint/UserWidget.h"
#include "FrogGameMode.generated.h"

UCLASS(minimalapi)
class AFrogGameMode : public AGameModeBase
{
	GENERATED_BODY()


public:
	AFrogGameMode();

	void SetWidgetVisibility();

	UFrogGameUI* GetGameHUD() const;


protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "User Interface", Meta = (BlueprintProtected = "true"))
	TSubclassOf<class UFrogGameUI> FrogHUDClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "User Interface")
	bool bShowWidget{true};
	UPROPERTY()
	class UFrogGameUI* CurrentWidget;


	void BeginPlay() override;
};
