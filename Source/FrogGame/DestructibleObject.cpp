// Fill out your copyright notice in the Description page of Project Settings.


#include "DestructibleObject.h"
#include "FrogGameCharacter.h"
#include "Engine/StaticMesh.h"
#include "SphereDrop.h"


// Sets default values
ADestructibleObject::ADestructibleObject()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ADestructibleObject::BeginPlay()
{
	Super::BeginPlay();
	StaticMesh = Cast<UStaticMeshComponent>(GetComponentByClass(UStaticMeshComponent::StaticClass()));
	StartTransform = GetTransform();
}

// Called every frame
void ADestructibleObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

float ADestructibleObject::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
                                      AActor* DamageCauser)
{
	const float ActualDamage{Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser)};
	UE_LOG(LogTemp, Warning, TEXT("Taking %f damage."), ActualDamage);
	if (ActualDamage > 0.f)
	{
		AFrogGameCharacter* Frog{Cast<AFrogGameCharacter>(DamageCauser)};
		// This needs to be more specific to the punches or we could just walk it to death
		if (Frog)
		{
			CurrentHealth -= ActualDamage;
			if (CurrentHealth <= 0.f && !IsActorBeingDestroyed() && !ShouldDestroy)
			{
				// TODO: Maybe set mass to 100kg once it loses all health, so it flies away only when punched to death
				if (StaticMesh)
				{
					StaticMesh->SetSimulatePhysics(true);
					StaticMesh->AddImpulse(CalculateImpulseVector(Frog));
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("Missing static mesh reference!"))
				}
				ShouldDestroy = true;
				GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ADestructibleObject::KillActor, 1.f,
				                                       false);
			}
		}
	}
	return ActualDamage;
}

void ADestructibleObject::SpawnSpheres() const
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
			Actor->GetWorld()->SpawnActor<ASphereDrop>(Drop, SpawnTransform);
		}
	}
}

FVector ADestructibleObject::CalculateImpulseVector(AFrogGameCharacter* Frog) const
{
	FVector ImpulseDirection{GetOwner()->GetActorLocation() - Frog->GetActorLocation()};
	ImpulseDirection.Normalize();
	return {ImpulseDirection * FlyAwayForce};
}

void ADestructibleObject::KillActor() const
{
	SpawnSpheres();
	GetOwner()->SetLifeSpan(0.001f);
}

FTransform ADestructibleObject::GetStartTransform()
{
	return StartTransform;
}
// Custom behaviour when saving or loading
void ADestructibleObject::ActorSaveDataSaved_Implementation()
{
}

void ADestructibleObject::ActorSaveDataLoaded_Implementation()
{
}