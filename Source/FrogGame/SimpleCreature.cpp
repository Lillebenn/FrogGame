// Fill out your copyright notice in the Description page of Project Settings.


#include "SimpleCreature.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/Controller.h"
#include "TargetingReticule.h"
#include "FrogGameInstance.h"
#include "TonguePivot.h"


// Sets default values
ASimpleCreature::ASimpleCreature()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CreatureMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Creature Mesh"));
	RootComponent = CreatureMesh;
		TongueTarget = CreateDefaultSubobject<UTonguePivot>(TEXT("Tongue Pivot Object"));
	TongueTarget->SetupAttachment(RootComponent);
	Reticule = CreateDefaultSubobject<UTargetingReticule>(TEXT("Targeting Reticule"));
	Reticule->SetupAttachment(RootComponent);

	MovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MovementComponent"));


	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

UTargetingReticule* ASimpleCreature::GetTargetingReticule_Implementation()
{
	return Reticule;
}


// Called when the game starts or when spawned
void ASimpleCreature::BeginPlay()
{
	Super::BeginPlay();
	//Reticule->InitWidget();
	//CalculateBoundingSize(); This was causing an error somewhere
	StartTransform = GetTransform();
	CreatureMesh->SetCollisionObjectType(ECC_GameTraceChannel1);
}

// Called every frame
void ASimpleCreature::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ASimpleCreature::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}


FEdibleInfo ASimpleCreature::GetInfo_Implementation() const
{
	return EdibleInfo;
}

void ASimpleCreature::DisableActor_Implementation()
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

UStaticMeshComponent* ASimpleCreature::GetMesh()
{
	return CreatureMesh;
}

UTonguePivot* ASimpleCreature::GetTargetComponent_Implementation()
{
	return TongueTarget;
}

void ASimpleCreature::OnDisabled_Implementation()
{
	DisableActor_Implementation();
	UFrogGameInstance* FrogInstance{Cast<UFrogGameInstance>(GetWorld()->GetGameInstance())};
	if (FrogInstance)
	{
		FrogInstance->OnActorDestroyed(this);
	}
}

// Custom behaviour when saving or loading
void ASimpleCreature::ActorSaveDataLoaded_Implementation()
{
}

void ASimpleCreature::ActorSaveDataSaved_Implementation()
{
}

void ASimpleCreature::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

FTransform ASimpleCreature::GetStartTransform()
{
	return StartTransform;
}

void ASimpleCreature::CalculateBoundingSize()
{
	if (CreatureMesh)
	{
		const FVector RoughSize = CreatureMesh->GetStaticMesh()->GetBoundingBox().GetSize();
		const FVector AbsoluteSize{RoughSize.GetAbsMin()};
		// Get the average axis value of the bounding box
		EdibleInfo.Size = (AbsoluteSize.X + AbsoluteSize.Y + AbsoluteSize.Z) / 3;
	}
}
