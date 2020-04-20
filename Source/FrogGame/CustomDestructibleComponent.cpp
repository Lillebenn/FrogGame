// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomDestructibleComponent.h"
#include "Engine/World.h"
#include "SphereDrop.h"
#include "FrogGameCharacter.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UCustomDestructibleComponent::UCustomDestructibleComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UCustomDestructibleComponent::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = MaxHealth;

	// ...
}


// Called every frame
void UCustomDestructibleComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                           FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

FVector UCustomDestructibleComponent::CalculateImpulseVector(AActor* Actor) const
{
	FVector ImpulseDirection{GetOwner()->GetActorLocation() - Actor->GetActorLocation()};
	ImpulseDirection.Normalize();
	return {ImpulseDirection * FlyAwayForce};
}

void UCustomDestructibleComponent::SpawnSpheres() const
{
	if (Drop)
	{
		for (int i{0}; i < NumDrops; i++)
		{
			const FVector2D SpawnLocation2D{FMath::RandPointInCircle(125.f)};
			const FVector ActorLocation{GetOwner()->GetActorLocation()};
			const FVector SpawnLocation{
				ActorLocation.X + SpawnLocation2D.X, ActorLocation.Y + SpawnLocation2D.Y, ActorLocation.Z
			};
			const FTransform SpawnTransform{SpawnLocation};
			GetWorld()->SpawnActor<ASphereDrop>(Drop, SpawnTransform);
		}
	}
}
void UCustomDestructibleComponent::KillActor()
{
	SpawnSpheres();
	if(DestructionSmoke)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DestructionSmoke, GetOwner()->GetActorLocation(), FRotator::ZeroRotator, FVector(SmokeScale));
	}
	GetOwner()->SetLifeSpan(0.001f);
}
