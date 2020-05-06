// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "FrogGameUI.h"
#include "FrogSaveGame.h"


#include "FrogGameInstance.generated.h"

struct FActorSaveData;
/**
 * 
 */
UCLASS()
class FROGGAME_API UFrogGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	explicit UFrogGameInstance(const FObjectInitializer& ObjectInitializer);
	
	UFUNCTION(BlueprintCallable)
	void NewHighScore(FString PlayerName, int HighScore, float PercentDestroyed, float TimeSpent) const;
	UFUNCTION(BlueprintCallable)
	FORCEINLINE TArray<FSaveInfo>& GetCurrentHighScores() const
	{
		return HighScoreSaveGame->HighScores;
	}

	UPROPERTY(EditAnywhere, Category = SaveGame)
	uint8 MaxNumHighScores{5};
	


private:
	UPROPERTY()
	UFrogSaveGame* HighScoreSaveGame;

	// Reference to the Grid
	class AGrid* CurrentGrid;
};
