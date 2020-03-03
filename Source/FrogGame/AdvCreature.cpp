// Fill out your copyright notice in the Description page of Project Settings.


#include "AdvCreature.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Controller.h"
#include "FrogGameInstance.h"
#include "TargetingReticule.h"
#include "Components/SkeletalMeshComponent.h"
#include "TonguePivot.h"

// Sets default values
AAdvCreature::AAdvCreature()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Reticule = CreateDefaultSubobject<UTargetingReticule>(TEXT("Targeting Reticule"));
	Reticule->SetupAttachment(GetMesh());
	TongueTarget = CreateDefaultSubobject<UTonguePivot>(TEXT("Tongue Pivot Object"));
	TongueTarget->SetupAttachment(RootComponent);

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

// Called when the game starts or when spawned
void AAdvCreature::BeginPlay()
{
	Super::BeginPlay();
	Reticule->InitWidget();

	StartTransform = GetTransform();
	GetCapsuleComponent()->SetCollisionObjectType(ECC_GameTraceChannel1);
	// For capsules we just use the radius value. Could potentially do a combination/average of the half-height and radius if the creature is particularly tall.
	CalculateBoundingSize();
	Reticule->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, AttachBoneName);
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


UTonguePivot* AAdvCreature::GetTargetComponent_Implementation()
{
	return TongueTarget;
}


// Custom behaviour when saving or loading
void AAdvCreature::ActorSaveDataLoaded_Implementation()
{
}

UTargetingReticule* AAdvCreature::GetTargetingReticule_Implementation()
{
	return Reticule;
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
	EdibleInfo.Size = (AbsoluteSize.X + AbsoluteSize.Y + AbsoluteSize.Z) / 6;
	EdibleInfo.SizeTier = IEdible::CalculateSizeTier(EdibleInfo.Size);
}
