// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
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
	void CreateNewSave(const FString& SaveName);
	void LoadSaveGame(const FString& SaveName);
	class UFrogSaveGame* LoadCurrentSave() const;
	void SaveCurrentToSlot() const;
	/**
	 * @brief Get all the save slot names in existence.
	 * We assume UserIndex will always be 0, so we can simply get a list of all the save slot names to display and load in a UMG Widget.
	 */
	TSet<FString> GetAllSaveNames() const;

	FString GetCurrentSaveName() const
	{
		return CurrentSaveName;
	}

	void SetCurrentSaveName(const FString& NewName) { CurrentSaveName = NewName; }

	void Shutdown() override;


private:
	UPROPERTY()
	FString CurrentSaveName;
	UPROPERTY()
	class UFrogSaveGame* CurrentSave{nullptr};
	UPROPERTY()
	class USaveSlotSettings* SaveInfo{nullptr};

	FActorSaveData SerializeActor(AActor* Actor) const;
	FSaveGameArchive ReadSaveData(const FActorSaveData& ActorRecord) const;

	void SaveActors(const FString& SaveSlotName) const;
	void LoadActors() const;
};
