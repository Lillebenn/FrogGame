// Fill out your copyright notice in the Description page of Project Settings.


#include "FrogGameInstance.h"
#include "Engine/World.h"
#include "FrogGameCharacter.h"
#include "Kismet/GameplayStatics.h"


void UFrogGameInstance::Init()
{
	HighScoreSaveGame = Cast<UFrogSaveGame>(UGameplayStatics::LoadGameFromSlot(TEXT("High Score List"), 0));
	if (!HighScoreSaveGame)
	{
		HighScoreSaveGame = Cast<UFrogSaveGame>(UGameplayStatics::CreateSaveGameObject(UFrogSaveGame::StaticClass()));
		HighScoreSaveGame->Construct(MaxNumHighScores);
	}
}

void UFrogGameInstance::NewHighScore(const FString PlayerName, const int HighScore,
                                     const float PercentDestroyed, const float TimeSpent) const
{
	TArray<FSaveInfo> HighScoreArray{HighScoreSaveGame->HighScores};
	FSaveInfo NewHighScore{PlayerName, HighScore, PercentDestroyed, TimeSpent};

	for (int i{0}; i < HighScoreArray.Num(); i++)
	{
		const FSaveInfo& CurrentHighScore{HighScoreArray[i]};
		if (HighScore >= CurrentHighScore.FinalScore)
		{
			HighScoreArray.EmplaceAt(i, NewHighScore);
			break;
		}
	}

	while (HighScoreArray.Num() > MaxNumHighScores)
	{
		HighScoreArray.Pop();
	}
	HighScoreSaveGame->HighScores = HighScoreArray;
}
