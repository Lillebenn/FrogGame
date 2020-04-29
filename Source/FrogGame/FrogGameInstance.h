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
	void CreateNewSave(const FString& SaveName);
	void LoadSaveGame(const FString& SaveName);
	class UFrogSaveGame* LoadCurrentSave() const;
	void SaveCurrentToSlot() const;


	void OnActorDestroyed(AActor* Actor) const;
	void OnActorMoved(AActor* Actor) const;

	void CreateCheckpoint();
	void LoadCheckpoint() const;
	void NewCheckpoint() const;
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

	//FORCEINLINE class UFrogGameUI* GetInGameUI() const { return InGameUI; }

	//void LoadInGameUI();
	//protected:
	//	// Dynamic reference to the blueprint class.
	//	TSubclassOf<class UUserWidget> InGameUIClass;
	//
	//	// Internal reference to the blueprint for gameplay logic
	//	UPROPERTY(BlueprintReadWrite, Category = "UI")
	//	class UFrogGameUI* InGameUI;
	UFrogSaveGame* GetCurrentSave() const
	{
		return CurrentSave;
	}

	static FActorSaveData SaveData(AActor* Actor);

private:

	// Reference to the Grid
	class AGrid* CurrentGrid;

	void Shutdown() override;


	UPROPERTY()
	FString CurrentSaveName;
	UPROPERTY()
	class UFrogSaveGame* CurrentSave{nullptr};
	UPROPERTY()
	class UFrogSaveGame* Checkpoint{nullptr};
	UPROPERTY()
	class USaveSlotSettings* SaveInfo{nullptr};


	void SaveActors(const FString& SaveSlotName) const;
	void LoadActors(UFrogSaveGame* SaveGame) const;
};
