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
	CreateCheckpoint();
}

void UFrogGameInstance::CreateNewSave(const FString& SaveName)
{
	CurrentSave = Cast<UFrogSaveGame>(UGameplayStatics::CreateSaveGameObject(UFrogSaveGame::StaticClass()));
	CurrentSaveName = SaveName;
	CurrentSave->SaveSlotName = CurrentSaveName;
	SaveInfo->LastSave = CurrentSaveName;

	SaveInfo->SaveSlotNames.Add(CurrentSaveName);
	UGameplayStatics::SaveGameToSlot(CurrentSave, CurrentSaveName, 0);
}


void UFrogGameInstance::LoadSaveGame(const FString& SaveName)
{
	if (UFrogSaveGame* LoadedGame{Cast<UFrogSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveName, 0))})
	{
		CurrentSave = LoadedGame;
		CurrentSaveName = SaveName;
		LoadCurrentSave(); // Actually load the info from the SaveGame file into the world. 
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to load save file: %s"), *SaveName);
	}
}

UFrogSaveGame* UFrogGameInstance::LoadCurrentSave() const
{
	LoadActors(GetCurrentSave());
	return Cast<UFrogSaveGame>(UGameplayStatics::LoadGameFromSlot(CurrentSaveName, 0));
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
	SaveCurrentToSlot();
	// Save the game before quitting. We always set the current save to be the one last saved to, so this should just update that file.
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
		SaveSlot->SavedActors.Add(SerializeActor(Actor));
		ISaveable::Execute_ActorSaveDataSaved(Actor);
	}
	//if (AFrogGameCharacter* Froggy{Cast<AFrogGameCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter())})
	//{
	//	SaveSlot->PlayerCharacter = SerializeActor(Froggy);
	//}
	// Execute some player logic when saving here if you want
	if (!UGameplayStatics::SaveGameToSlot(SaveSlot, SaveSlotName, 0))
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to save file to slot!"));
	}
}

void UFrogGameInstance::LoadActors(UFrogSaveGame* SaveGame) const
{
	for (FActorSaveData ActorRecord : SaveGame->SavedActors)
	{
		FVector SpawnPos = ActorRecord.ActorTransform.GetLocation();
		FRotator SpawnRot = ActorRecord.ActorTransform.Rotator();
		FActorSpawnParameters SpawnParams;
		SpawnParams.Name = ActorRecord.ActorName;
		UClass* SpawnClass = FindObject<UClass>(ANY_PACKAGE, *ActorRecord.ActorClass);
		if (SpawnClass)
		{
			AActor* NewActor = GetWorld()->SpawnActor(SpawnClass, &SpawnPos, &SpawnRot, SpawnParams);
			FSaveGameArchive Ar{ReadSaveData(ActorRecord)};
			NewActor->Serialize(Ar);
			NewActor->SetActorTransform(ActorRecord.ActorTransform);
			ISaveable::Execute_ActorSaveDataLoaded(NewActor);
		}
	}
	// Load data to the frog character here.
	//AFrogGameCharacter* Froggy{Cast<AFrogGameCharacter>(GetPrimaryPlayerController()->GetCharacter())};
	//FSaveGameArchive Ar{ReadSaveData(CurrentSave->PlayerCharacter)};
	//Froggy->Serialize(Ar);
	//Froggy->SetActorTransform(CurrentSave->PlayerCharacter.ActorTransform);
}

FSaveGameArchive UFrogGameInstance::ReadSaveData(const FActorSaveData& ActorRecord) const
{
	FMemoryReader MemoryReader(ActorRecord.ActorData, true);
	FSaveGameArchive Ar(MemoryReader);
	return Ar;
}

FActorSaveData UFrogGameInstance::SerializeActor(AActor* Actor) const
{
	FActorSaveData ActorRecord;
	ActorRecord.ActorName = FName(*Actor->GetName());
	ActorRecord.ActorClass = Actor->GetClass()->GetPathName();
	ActorRecord.ActorTransform = Actor->GetTransform();

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
		Checkpoint->PlayerCharacter = SerializeActor(Froggy);
	}
}

void UFrogGameInstance::LoadCheckpoint() const
{
	if (Checkpoint)
	{
		LoadActors(Checkpoint);
	}
}

void UFrogGameInstance::OnNewCheckpoint() const
{
	if (Checkpoint)
	{
		Checkpoint->SavedActors.Empty();
		if (AFrogGameCharacter* Froggy = Cast<AFrogGameCharacter>(
			GetWorld()->GetFirstPlayerController()->GetCharacter()))
		{
			Checkpoint->PlayerCharacter = SerializeActor(Froggy);
		}
	}
}

void UFrogGameInstance::OnActorDestroyed(AActor* Actor) const
{
	if (Checkpoint)
	{
		Checkpoint->SavedActors.Add(SerializeActor(Actor));
		ISaveable::Execute_ActorSaveDataSaved(Actor);
	}
}
