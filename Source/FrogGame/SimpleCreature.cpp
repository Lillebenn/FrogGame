// Fill out your copyright notice in the Description page of Project Settings.


#include "SimpleCreature.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Components/CapsuleComponent.h"


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

	EdibleInfo.RoughSize = FVector(Radius * 2.0f);
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

// Called when the game starts or when spawned
void ASimpleCreature::BeginPlay()
{
	Super::BeginPlay();
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

void ASimpleCreature::Consume_Implementation(float FrogSize, const FString& BoneName)
{
}


FVector ASimpleCreature::GetSize() const
{
	return EdibleInfo.RoughSize;
}
