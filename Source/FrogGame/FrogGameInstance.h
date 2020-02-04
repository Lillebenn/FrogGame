// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "FrogGameUI.h"
#include "FrogGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class FROGGAME_API UFrogGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
protected:
	// Dynamic reference to the blueprint class.
	TSubclassOf<class UUserWidget> InGameUIClass;

	// Internal reference to the blueprint for gameplay logic
	UPROPERTY(BlueprintReadWrite, Category = "UI")
	class UFrogGameUI* InGameUI;

public:

	UFrogGameInstance(const FObjectInitializer & ObjectInitialzer);

	FORCEINLINE class UFrogGameUI* GetInGameUI() const { return InGameUI; }


public:

	void LoadIngameUI();


private:

	// Reference to the Grid
	class AGrid* CurrentGrid;

};
