// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SaveSlotSettings.generated.h"

/**
 * 
 */
UCLASS()
class FROGGAME_API USaveSlotSettings : public USaveGame
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere, Category = Basic)
	FString SaveSlotName{TEXT("SaveSlotSettings")};

	UPROPERTY(VisibleAnywhere, Category = Basic)
	uint32 UserIndex{0};

	UPROPERTY(VisibleAnywhere, Category = Basic)
	TSet<FString> SaveSlotNames;
	UPROPERTY(VisibleAnywhere, Category = Basic)
	FString LastSave;
};
