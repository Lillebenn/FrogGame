// Fill out your copyright notice in the Description page of Project Settings.


#include "EdibleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "SphereDrop.h"
#include "FrogGameCharacter.h"

// Sets default values for this component's properties
UEdibleComponent::UEdibleComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UEdibleComponent::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = Health;

	// ...
}


// Called every frame
void UEdibleComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                     FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UEdibleComponent::SpawnSpheres() const
{
	AActor* Actor{GetOwner()};
	UE_LOG(LogTemp, Warning, TEXT("Owner is %s"), *Actor->GetName())
	if (Drop)
	{
		for (int i{0}; i < NumDrops; i++)
		{
			const FVector2D SpawnLocation2D{FMath::RandPointInCircle(125.f)};
			const FVector ActorLocation{Actor->GetActorLocation()};
			const FVector SpawnLocation{
				ActorLocation.X + SpawnLocation2D.X, ActorLocation.Y + SpawnLocation2D.Y, ActorLocation.Z
			};
			const FTransform SpawnTransform{SpawnLocation};
			ASphereDrop* Sphere{Actor->GetWorld()->SpawnActorDeferred<ASphereDrop>(Drop, SpawnTransform)};
			Sphere->EdibleComponent = DuplicateObject(this, StaticClass());
			UGameplayStatics::FinishSpawningActor(Sphere, SpawnTransform);
		}
	}
}

FVector UEdibleComponent::CalculateImpulseVector(AFrogGameCharacter* Frog) const
{
	FVector ImpulseDirection{GetOwner()->GetActorLocation() - Frog->GetActorLocation()};
	ImpulseDirection.Normalize();
	return {ImpulseDirection * FlyAwayForce};
}

void UEdibleComponent::KillActor() const
{
	SpawnSpheres();
	GetOwner()->SetLifeSpan(0.001f);
}
