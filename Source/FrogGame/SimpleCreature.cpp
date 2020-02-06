// Fill out your copyright notice in the Description page of Project Settings.


#include "SimpleCreature.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Controller.h"
#include "FrogGameInstance.h"


// Sets default values
ASimpleCreature::ASimpleCreature()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	NavCollider = CreateDefaultSubobject<UCapsuleComponent>(TEXT("NavCollider"));
	const float CapsuleHeight{80.0f};
	const float Radius{15.f};
	NavCollider->InitCapsuleSize(Radius, CapsuleHeight);
	NavCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = NavCollider;

	CreatureMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Creature Mesh"));
	CreatureMesh->SetupAttachment(NavCollider);
	CreatureMesh->SetRelativeLocation(FVector(0, 0, (CapsuleHeight / 2.0f) + 10.0f));

	MovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MovementComponent"));

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	// TODO: Make sure to disable AI Controller when grabbed by tongue.
}

// Called when the game starts or when spawned
void ASimpleCreature::BeginPlay()
{
	Super::BeginPlay();
	CalculateBoundingSize();
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
		AI->UnPossess();
	}
}

USceneComponent* ASimpleCreature::GetTargetComponent_Implementation()
{
	return CreatureMesh;
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
	UFrogGameInstance* FrogInstance{Cast<UFrogGameInstance>(GetWorld()->GetGameInstance())};
	if(FrogInstance)
	{
		FrogInstance->OnActorDestroyed(this);
	}
	Super::EndPlay(EndPlayReason);
}

void ASimpleCreature::CalculateBoundingSize()
{
	const FVector RoughSize = CreatureMesh->GetStaticMesh()->GetBoundingBox().GetSize();
	const FVector AbsoluteSize{RoughSize.GetAbsMin()};
	// Get the average axis value of the bounding box
	EdibleInfo.Size = (AbsoluteSize.X + AbsoluteSize.Y + AbsoluteSize.Z) / 3;
}
