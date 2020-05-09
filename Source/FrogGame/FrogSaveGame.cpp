// Fill out your copyright notice in the Description page of Project Settings.


#include "FrogSaveGame.h"

void UFrogSaveGame::Construct(int NumHighScores)
{
	for (int i{0}; i < NumHighScores; i++)
	{
		HighScores.Add(FSaveInfo());
	}
}
