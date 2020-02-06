// Fill out your copyright notice in the Description page of Project Settings.


#include "FrogGameInstance.h"
#include "SaveSlotSettings.h"
#include "Kismet/GameplayStatics.h"
#include "FrogSaveGame.h"
#include "Saveable.h"
#include "SaveSlotSettings.h"
#include "Engine/World.h"
#include "FrogGameCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "FrogGameMode.h"


UFrogGameInstance::UFrogGameInstance(const FObjectInitializer& ObjectInitializer) : UGameInstance(ObjectInitializer)
{
	SaveInfo = Cast<USaveSlotSettings>(UGameplayStatics::LoadGameFromSlot("SaveSlotSettings", 0));
	// Load the save slot info.
	if (SaveInfo)
	{
		CurrentSaveName = SaveInfo->LastSave;
		CurrentSave = Cast<UFrogSaveGame>(UGameplayStatics::LoadGameFromSlot(CurrentSaveName, 0));
		if (!CurrentSave)
		{
			CreateNewSave("New Save");
		}
	}
	else // Save slot info doesn't exist, so we create it
	{
		SaveInfo = Cast<USaveSlotSettings>(UGameplayStatics::CreateSaveGameObject(USaveSlotSettings::StaticClass()));
		const bool Success{UGameplayStatics::SaveGameToSlot(SaveInfo, TEXT("SaveSlotSettings"), 0)};
		if (!Success)
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to create save slot file!"));
		}
		else
		{
			CreateNewSave("New Save");
		}
	}
	// TODO: Move logic to GameMode with a boolean in game instance to tell the game mode if the game was just launched
	// not sure if needed
	// GameMode BeginPlay should hopefully be able to handle loading in game state info after the level has been fully loaded.
}

void UFrogGameInstance::CreateNewSave(const FString& SaveName)
{
	CurrentSave = Cast<UFrogSaveGame>(UGameplayStatics::CreateSaveGameObject(UFrogSaveGame::StaticClass()));
	CurrentSaveName = SaveName;
	CurrentSave->SaveSlotName = CurrentSaveName;
	CurrentSave->LevelName = FName(*UGameplayStatics::GetCurrentLevelName(GetWorld()));
	SaveInfo->LastSave = CurrentSaveName;

	SaveInfo->SaveSlotNames.Add(CurrentSaveName);
	SaveActors(SaveName);
	UGameplayStatics::SaveGameToSlot(CurrentSave, CurrentSave->SaveSlotName, CurrentSave->UserIndex);
}


void UFrogGameInstance::LoadSaveGame(const FString& SaveName)
{
	if (UFrogSaveGame* LoadedGame{Cast<UFrogSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveName, 0))})
	{
		CurrentSave = LoadedGame;
		CurrentSaveName = SaveName;
		if (CurrentSave->LevelName != FName(*UGameplayStatics::GetCurrentLevelName(GetWorld())))
			// if the level name is different from the one we're in
		{
			UGameplayStatics::OpenLevel(GetWorld(), CurrentSave->LevelName);

			if (AFrogGameMode* GameMode{Cast<AFrogGameMode>(GetWorld()->GetAuthGameMode())})
			{
				GameMode->bNewLevel = true;
				// This should prepare the game mode to load the current save state once the level is loaded
			}
		}
		else
		{
			GetWorld()->GetAuthGameMode()->ResetLevel();
			LoadCurrentSave();
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to load save file: %s"), *SaveName);
	}
}

void UFrogGameInstance::LoadCurrentSave() const
{
	UGameplayStatics::LoadGameFromSlot(CurrentSaveName, 0);

	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsWithInterface(GetWorld(), USaveable::StaticClass(), Actors);
	for (auto Actor : Actors)
	{
		bool bExists{false};

		for (auto i{0}; i < CurrentSave->SavedActors.Num(); i++)
		{
			FActorSaveData ActorRecord{CurrentSave->SavedActors[i]};
			if (FName(*Actor->GetName()) == ActorRecord.ActorName) // Check if the actor is in the list of saved actors
			{
				bExists = true;
				FMemoryReader MemoryReader(ActorRecord.ActorData, true);
				FSaveGameArchive Ar(MemoryReader);
				Actor->Serialize(Ar);
				Actor->SetActorTransform(ActorRecord.ActorTransform);
				ISaveable::Execute_ActorSaveDataLoaded(Actor);
				CurrentSave->SavedActors.RemoveAt(i);
				break;
			}
		}
		if (!bExists)
			// Couldn't find the actor in the list of saved actors, this means it was dead when the save occurred.
		{
			Actor->Destroy();
		}
	}
	LoadPlayerInfo(CurrentSave);
}

void UFrogGameInstance::SaveCurrentToSlot() const
{
	if (CurrentSave)
	{
		SaveActors(CurrentSaveName);
		// Maybe save some other info as well.
	}
}


TSet<FString> UFrogGameInstance::GetAllSaveNames() const
{
	return SaveInfo->SaveSlotNames;
}

void UFrogGameInstance::Shutdown()
{
	// I have a feeling actors may be destroyed before this is called, but putting this here for now.
	//SaveCurrentToSlot();
	// Save the game before quitting. We always set the current save to be the one last saved to, so this should just update that file.
}


void UFrogGameInstance::LoadPlayerInfo(UFrogSaveGame* SaveGame) const
{
	AFrogGameCharacter* Froggy{Cast<AFrogGameCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter())};
	if (Froggy)
	{
		FActorSaveData PlayerRecord{SaveGame->PlayerCharacter};
		FMemoryReader MemoryReader(PlayerRecord.ActorData, true);
		FSaveGameArchive Ar(MemoryReader);
		Froggy->Serialize(Ar);
		Froggy->SetActorTransform(PlayerRecord.ActorTransform);
	}
}

void UFrogGameInstance::SaveActors(const FString& SaveSlotName) const
{
	UFrogSaveGame* SaveSlot{Cast<UFrogSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0))};
	if (!SaveSlot)
	{
		return;
	}
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsWithInterface(GetWorld(), USaveable::StaticClass(), Actors);
	SaveSlot->SavedActors.Empty(); // Clear the array so we don't get duplicates.
	for (auto Actor : Actors)
	{
		SaveSlot->SavedActors.Add(SaveData(Actor));
		ISaveable::Execute_ActorSaveDataSaved(Actor);
	}
	/*if (AFrogGameCharacter* Froggy{Cast<AFrogGameCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter())})
	{
		SaveSlot->PlayerCharacter = SaveData(Froggy);
	}*/
	// Execute some player logic when saving here if you want
	if (!UGameplayStatics::SaveGameToSlot(SaveSlot, SaveSlot->SaveSlotName, SaveSlot->UserIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to save file to slot!"));
	}
}


void UFrogGameInstance::LoadRespawnActors(UFrogSaveGame* SaveGame) const
{
	for (FActorSaveData ActorRecord : SaveGame->SavedActors)
	{
		FVector SpawnPos = ActorRecord.ActorTransform.GetLocation();
		FRotator SpawnRot = ActorRecord.ActorTransform.GetRotation().Rotator();
		FActorSpawnParameters SpawnParams;
		SpawnParams.Name = ActorRecord.ActorName;
		UClass* SpawnClass = FindObject<UClass>(ANY_PACKAGE, *ActorRecord.ActorClass);
		if (SpawnClass)
		{
			AActor* NewActor = GetWorld()->SpawnActor(SpawnClass, &SpawnPos, &SpawnRot, SpawnParams);
			FMemoryReader MemoryReader(ActorRecord.ActorData, true);
			FSaveGameArchive Ar(MemoryReader);
			NewActor->Serialize(Ar);
			ISaveable::Execute_ActorSaveDataLoaded(NewActor);
		}
	}
	LoadPlayerInfo(Checkpoint);
}


FActorSaveData UFrogGameInstance::SaveData(AActor* Actor)
{
	FActorSaveData ActorRecord;
	ActorRecord.ActorClass = Actor->GetClass()->GetPathName();
	ActorRecord.ActorName = FName(*Actor->GetName());
	if (Actor->Implements<USaveable>())
	{
		ActorRecord.ActorTransform = ISaveable::Execute_GetStartTransform(Actor);
	}
	else
	{
		ActorRecord.ActorTransform = Actor->GetTransform();
	}

	FMemoryWriter MemoryWriter(ActorRecord.ActorData, true);
	FSaveGameArchive Ar(MemoryWriter);
	Actor->Serialize(Ar);
	return ActorRecord;
}

void UFrogGameInstance::CreateCheckpoint()
{
	Checkpoint = Cast<UFrogSaveGame>(UGameplayStatics::CreateSaveGameObject(UFrogSaveGame::StaticClass()));
	Checkpoint->SaveSlotName = TEXT("Checkpoint");
	if (AFrogGameCharacter* Froggy = Cast<AFrogGameCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter()))
	{
		Checkpoint->PlayerCharacter = SaveData(Froggy);
	}
}

void UFrogGameInstance::LoadCheckpoint() const
{
	if (Checkpoint)
	{
		// Find existing actors with the Saveable interface and reset them.
		TArray<AActor*> Actors;
		UGameplayStatics::GetAllActorsWithInterface(GetWorld(), USaveable::StaticClass(), Actors);
		for (auto Actor : Actors)
		{
			ISaveable::Execute_ResetActor(Actor);
		}
		// Respawn any actors that were destroyed in the time since last checkpoint.
		LoadRespawnActors(Checkpoint);
	}
	Checkpoint->SavedActors.Empty();
}

void UFrogGameInstance::OnNewCheckpoint() const
{
	if (Checkpoint)
	{
		Checkpoint->SavedActors.Empty();
		if (AFrogGameCharacter* Froggy = Cast<AFrogGameCharacter>(
			GetWorld()->GetFirstPlayerController()->GetCharacter()))
		{
			Checkpoint->PlayerCharacter = SaveData(Froggy);
		}
	}
}

void UFrogGameInstance::OnActorDestroyed(AActor* Actor) const
{
	if (Checkpoint)
	{
		Checkpoint->SavedActors.Add(SaveData(Actor));
		ISaveable::Execute_ActorSaveDataSaved(Actor);
	}
}
