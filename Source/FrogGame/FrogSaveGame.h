// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "FrogSaveGame.generated.h"

USTRUCT(BlueprintType)
struct FSaveInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString PlayerName{TEXT("N/A")};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int FinalScore{0};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PercentWorldDestroyed{0.f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TimeSpent{0.f};
};

/**
 * 
 */
UCLASS()
class FROGGAME_API UFrogSaveGame : public USaveGame
{
	GENERATED_BODY()
public:
	UPROPERTY()
	FString SaveSlotName{TEXT("High Score List")};

	UPROPERTY()
	TArray<FSaveInfo> HighScores;

	UPROPERTY()
	uint32 UserIndex{0};
};
