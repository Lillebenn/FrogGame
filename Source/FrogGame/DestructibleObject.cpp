// Fill out your copyright notice in the Description page of Project Settings.


#include "DestructibleObject.h"
#include "FrogGameCharacter.h"
#include "Engine/StaticMesh.h"
#include "SphereDrop.h"


// Sets default values
ADestructibleObject::ADestructibleObject()
{
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetCollisionProfileName(TEXT("Edible"));
	RootComponent = StaticMesh;
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ADestructibleObject::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = MaxHealth;
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
		if (Frog)
		{
			CurrentHealth -= ActualDamage;
			if (CurrentHealth <= 0.f)
			{
				if (StaticMesh)
				{
					StaticMesh->SetMassOverrideInKg(NAME_None, 100.f);
					StaticMesh->SetSimulatePhysics(true);
					StaticMesh->AddImpulse(CalculateImpulseVector(Frog));
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("Missing static mesh reference!"))
				}
				GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ADestructibleObject::KillActor, 3.f,
				                                       false);
			}
		}
	}
	return ActualDamage;
}


FVector ADestructibleObject::CalculateImpulseVector(AFrogGameCharacter* Frog) const
{
	FVector ImpulseDirection{GetActorLocation() - Frog->GetActorLocation()};
	ImpulseDirection.Normalize();
	return {ImpulseDirection * FlyAwayForce};
}

void ADestructibleObject::KillActor()
{
	SpawnSpheres();
	SetLifeSpan(0.001f);
}

void ADestructibleObject::SpawnSpheres() const
{
	if (Drop)
	{
		for (int i{0}; i < NumDrops; i++)
		{
			const FVector2D SpawnLocation2D{FMath::RandPointInCircle(125.f)};
			const FVector ActorLocation{GetActorLocation()};
			const FVector SpawnLocation{
				ActorLocation.X + SpawnLocation2D.X, ActorLocation.Y + SpawnLocation2D.Y, ActorLocation.Z
			};
			const FTransform SpawnTransform{SpawnLocation};
			GetWorld()->SpawnActor<ASphereDrop>(Drop, SpawnTransform);
		}
	}
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
