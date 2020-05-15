// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomDestructibleComponent.h"

#include "EdibleComponent.h"
#include "Engine/World.h"
#include "SphereDrop.h"
#include "FrogGameCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "FrogGameMode.h"

// Sets default values for this component's properties
UCustomDestructibleComponent::UCustomDestructibleComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	static ConstructorHelpers::FClassFinder<ASphereDrop> SphereDrop(TEXT("/Game/Blueprints/Environment/BP_SphereDrop"));
	Drop = SphereDrop.Class;
	// ...
}


// Called when the game starts
void UCustomDestructibleComponent::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = MaxHealth;
	GameMode = Cast<AFrogGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
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
		const auto Edible{GetOwner()->FindComponentByClass<UEdibleComponent>()};
		for (int i{0}; i < NumDrops; i++)
		{
			const FVector2D SpawnLocation2D{FMath::RandPointInCircle(125.f)};
			const FVector ActorLocation{GetOwner()->GetActorLocation()};
			const FVector SpawnLocation{
				ActorLocation.X + SpawnLocation2D.X, ActorLocation.Y + SpawnLocation2D.Y, ActorLocation.Z
			};
			ASphereDrop* Sphere{GameMode->SpawnAvailableSphere(SpawnLocation)};
			if (Edible && Sphere)
			{
				Sphere->EdibleComponent->ScorePoints = Edible->ScorePoints / NumDrops;
				Sphere->EdibleComponent->PowerPoints = Edible->PowerPoints / NumDrops;
			}
			else
			{
				// No available spheres found, don't bother looking for more.
				break;
			}
		}
	}
}

void UCustomDestructibleComponent::KillActor() const
{
	if (DestructionSmoke)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DestructionSmoke, GetOwner()->GetActorLocation(),
		                                         FRotator::ZeroRotator, FVector(SmokeScale));
	}
	if (bSpawnSphereDrops)
	{
		SpawnSpheres();
		GetOwner()->SetLifeSpan(0.001f);
	}
	else
	{
		Cast<AFrogGameCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter())->Consume(GetOwner());
	}
}
