// Fill out your copyright notice in the Description page of Project Settings.


#include "AdvCreature.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Controller.h"
#include "FrogGameInstance.h"
#include "Components/SkeletalMeshComponent.h"

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
	AController* AI{GetController()};
	if (AI)
	{
		AI->StopMovement();
		AI->UnPossess();
		AI->Destroy();
	}
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


USceneComponent* AAdvCreature::GetTargetComponent_Implementation()
{
	return GetCapsuleComponent();
}

// Custom behaviour when saving or loading
void AAdvCreature::ActorSaveDataLoaded_Implementation()
{
}

FTransform AAdvCreature::GetStartTransform()
{
	return StartTransform;
}

void AAdvCreature::ActorSaveDataSaved_Implementation()
{
}
void AAdvCreature::CalculateBoundingSize()
{
	const FVector RoughSize = GetMesh()->Bounds.GetBox().GetSize();
	const FVector AbsoluteSize{RoughSize.GetAbsMin()};
	// Get the average axis value of the bounding box
	EdibleInfo.Size = (AbsoluteSize.X + AbsoluteSize.Y + AbsoluteSize.Z) / 3;
}