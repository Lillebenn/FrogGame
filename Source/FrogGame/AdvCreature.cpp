// Fill out your copyright notice in the Description page of Project Settings.


#include "AdvCreature.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Controller.h"
#include "FrogGameInstance.h"

// Sets default values
AAdvCreature::AAdvCreature()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// For capsules we just use the radius value. Could potentially do a combination/average of the half-height and radius if the creature is particularly tall.
	EdibleInfo.Size = GetCapsuleComponent()->GetScaledCapsuleRadius();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

// Called when the game starts or when spawned
void AAdvCreature::BeginPlay()
{
	Super::BeginPlay();

	StartTransform = GetTransform();
}

// Called every frame
void AAdvCreature::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AAdvCreature::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}


FEdibleInfo AAdvCreature::GetInfo_Implementation() const
{
	return EdibleInfo;
}

void AAdvCreature::DisableActor_Implementation()
{
	// Not 100% sure if this is necessary, but we don't need the AI to keep running after being snatched.
	AController* AI{GetController()};
	if (AI)
	{
		AI->StopMovement();
		AI->UnPossess();
		AI->Destroy();
	}
}

USceneComponent* AAdvCreature::GetTargetComponent_Implementation()
{
	return GetCapsuleComponent();
}

// Custom behaviour when saving or loading
void AAdvCreature::ActorSaveDataLoaded_Implementation()
{
}

FTransform AAdvCreature::GetStartTransform_Implementation()
{
	return StartTransform;
}

void AAdvCreature::ResetActor_Implementation()
{
	AController* AI{GetController()};
	AI->Reset();
	SetActorTransform(StartTransform);
}

void AAdvCreature::OnDisabled_Implementation()
{
	DisableActor_Implementation();
	UFrogGameInstance* FrogInstance{Cast<UFrogGameInstance>(GetWorld()->GetGameInstance())};
	if (FrogInstance)
	{
		FrogInstance->OnActorDestroyed(this);
	}
}

void AAdvCreature::ActorSaveDataSaved_Implementation()
{
}
