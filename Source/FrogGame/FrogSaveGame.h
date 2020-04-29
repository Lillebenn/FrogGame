// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "FrogSaveGame.generated.h"

USTRUCT()
struct FActorSaveData
{
	GENERATED_USTRUCT_BODY()

	FString ActorClass;
	FName ActorName;
	FTransform ActorTransform;
	TArray<uint8> ActorData;

	friend FArchive& operator<<(FArchive& Ar, FActorSaveData& ActorData)
	{
		Ar << ActorData.ActorClass;
		Ar << ActorData.ActorName;
		Ar << ActorData.ActorTransform;
		Ar << ActorData.ActorData;
		return Ar;
	}
};

struct FSaveGameArchive : public FObjectAndNameAsStringProxyArchive
{
	FSaveGameArchive(FArchive& InInnerArchive)
		: FObjectAndNameAsStringProxyArchive(InInnerArchive, true)
	{
		ArIsSaveGame = true;
	}
};

struct FPlayerData
{
};

/**
 * 
 */
UCLASS()
class FROGGAME_API UFrogSaveGame : public USaveGame
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere, Category = Basic)
	FString SaveSlotName{TEXT("New Save")};

	UPROPERTY(VisibleAnywhere, Category = Basic)
	uint32 UserIndex{0};

	TArray<AActor*> MovedActors;
	TArray<FActorSaveData> SavedActors;
	FActorSaveData PlayerCharacter;
};
