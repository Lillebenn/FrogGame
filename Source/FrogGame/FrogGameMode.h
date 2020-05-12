// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FrogGameUI.h"
#include "GameFramework/GameModeBase.h"

#include "Blueprint/UserWidget.h"
#include "FrogGameMode.generated.h"

UENUM(BlueprintType)
enum class ECurrentWidget : uint8
{
	MainMenu,
	InGameHUD,
	PauseMenu,
	EndScreen
};

UCLASS(minimalapi)
class AFrogGameMode : public AGameModeBase
{
	GENERATED_BODY()


public:

	UFUNCTION(BlueprintCallable)
	void SetVisibleWidget(TEnumAsByte<ECurrentWidget> NewVisibleWidget);

	class UFrogGameUI* GetInGameHUD();

	void SetCurrentWidgetVisibility();

	FORCEINLINE ECurrentWidget GetCurrentWidget() const
	{
		return CurrentWidget;
	}

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "User Interface", Meta = (BlueprintProtected = "true"))
	TSubclassOf<class UFrogGameUI> FrogHUDClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "User Interface", Meta = (BlueprintProtected = "true"))
	TSubclassOf<class UFrogGameUI> MainMenuClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "User Interface", Meta = (BlueprintProtected = "true"))
	TSubclassOf<class UFrogGameUI> PauseMenuClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "User Interface", Meta = (BlueprintProtected = "true"))
	TSubclassOf<class UFrogGameUI> EndScreenClass;

	UPROPERTY(BlueprintReadOnly)
	class UFrogGameUI* FrogHUDWidget;
	UPROPERTY(BlueprintReadOnly)
	class UFrogGameUI* MainMenuWidget;
	UPROPERTY(BlueprintReadOnly)
	class UFrogGameUI* PauseMenuWidget;
	UPROPERTY(BlueprintReadOnly)
	class UFrogGameUI* EndScreenWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USoundBase* RegularModeMusic;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USoundBase* PowerModeMusic;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USoundBase* MainMenuMusic;


	UPROPERTY(BlueprintReadWrite)
	class UAudioComponent* RegularModeAudio;
	UPROPERTY(BlueprintReadWrite)
	class UAudioComponent* PowerModeAudio;
	UPROPERTY(BlueprintReadWrite)
	class UAudioComponent* MainMenuAudio;

	UFUNCTION(BlueprintImplementableEvent)
	void OpenedMainMenu();
	UFUNCTION(BlueprintImplementableEvent)
	void OpenedEndScreen();


	bool bIsBeginPlay{true};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "User Interface")
	ECurrentWidget CurrentWidget{ECurrentWidget::MainMenu};
	class UFrogGameUI* ConstructWidget(TSubclassOf<class UFrogGameUI> Subclass) const;
	void BeginPlay() override;
private:
	ESlateVisibility GetNewVisibility(class UFrogGameUI* WidgetToSet);
};
