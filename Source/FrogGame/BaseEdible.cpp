// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseEdible.h"
#include "Engine/StaticMesh.h"

ABaseEdible::ABaseEdible()
{
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetCollisionObjectType(ECC_GameTraceChannel1);

	RootComponent = StaticMesh;
}

void ABaseEdible::Tick(float DeltaTime)
{
}


FEdibleInfo ABaseEdible::GetInfo_Implementation() const
{
	return EdibleInfo;
}

void ABaseEdible::DisableActor_Implementation()
{
}

USceneComponent* ABaseEdible::GetTargetComponent_Implementation()
{
	return StaticMesh;
}

// Custom behaviour when saving or loading
void ABaseEdible::ActorSaveDataSaved_Implementation()
{
}

void ABaseEdible::ActorSaveDataLoaded_Implementation()
{
}

FTransform ABaseEdible::GetStartTransform()
{
	return GetTransform();
}

void ABaseEdible::BeginPlay()
{
	Super::BeginPlay();
}
