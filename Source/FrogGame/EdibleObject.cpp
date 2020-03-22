// Fill out your copyright notice in the Description page of Project Settings.

#include "EdibleObject.h"
#include "Engine/StaticMesh.h"
#include "EdibleComponent.h"


AEdibleObject::AEdibleObject()
{
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetCollisionObjectType(ECC_GameTraceChannel1);
	EdibleComponent = CreateDefaultSubobject<UEdibleComponent>(TEXT("Edible Info"));

	RootComponent = StaticMesh;
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

FTransform AEdibleObject::GetStartTransform()
{
	return StartTransform;
}



void AEdibleObject::BeginPlay()
{
	Super::BeginPlay();
	StartTransform = GetTransform();
}

