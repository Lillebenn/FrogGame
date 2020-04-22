// Fill out your copyright notice in the Description page of Project Settings.

#include "EdibleObject.h"
#include "Engine/StaticMesh.h"
#include "EdibleComponent.h"


AEdibleObject::AEdibleObject()
{
	EdibleComponent->bAllowSuction = true;
}

void AEdibleObject::Tick(float DeltaTime)
{
}


bool AEdibleObject::IsDisabled_Implementation()
{
	return bShouldDestroy;
}



// Custom behaviour when saving or loading
void AEdibleObject::ActorSaveDataSaved_Implementation()
{
}

void AEdibleObject::ActorSaveDataLoaded_Implementation()
{
}




void AEdibleObject::BeginPlay()
{
	Super::BeginPlay();
	StartTransform = GetTransform();
}
