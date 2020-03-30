// Fill out your copyright notice in the Description page of Project Settings.

#include "EdibleObject.h"
#include "Engine/StaticMesh.h"
#include "EdibleComponent.h"


AEdibleObject::AEdibleObject()
{
	EdibleComponent = CreateDefaultSubobject<UEdibleComponent>(TEXT("Edible Info"));
}

void AEdibleObject::Tick(float DeltaTime)
{
}


bool AEdibleObject::IsDisabled_Implementation()
{
	return bShouldDestroy;
}

void AEdibleObject::IgnorePawnCollision_Implementation()
{
	StaticMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	StaticMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
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
