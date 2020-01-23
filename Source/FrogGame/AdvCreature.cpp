// Fill out your copyright notice in the Description page of Project Settings.


#include "AdvCreature.h"
#include "Components/CapsuleComponent.h"
// Sets default values
AAdvCreature::AAdvCreature()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->GetScaledCapsuleSize(EdibleInfo.RoughSize.X, EdibleInfo.RoughSize.Y);
	EdibleInfo.RoughSize.Z = EdibleInfo.RoughSize.X;
}

// Called when the game starts or when spawned
void AAdvCreature::BeginPlay()
{
	Super::BeginPlay();
	
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

void AAdvCreature::Consume(float FrogSize, const FString& BoneName)
{
}

FVector AAdvCreature::GetSize() const
{
	return EdibleInfo.RoughSize;
}

