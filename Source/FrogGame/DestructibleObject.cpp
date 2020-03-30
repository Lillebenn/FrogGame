// Fill out your copyright notice in the Description page of Project Settings.


#include "DestructibleObject.h"

#include "CustomDestructibleComponent.h"
#include "FrogGameCharacter.h"
#include "Engine/StaticMesh.h"
#include "SphereDrop.h"


// Sets default values
ADestructibleObject::ADestructibleObject()
{
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetCollisionProfileName(TEXT("Edible"));
	RootComponent = StaticMesh;
	DestructibleComponent = CreateDefaultSubobject<UCustomDestructibleComponent>(TEXT("Destructible"));
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ADestructibleObject::BeginPlay()
{
	Super::BeginPlay();
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
			float& CurrentHealth{DestructibleComponent->CurrentHealth};
			CurrentHealth -= ActualDamage;
			if (CurrentHealth <= 0.f)
			{
				if (StaticMesh)
				{
					StaticMesh->SetMassOverrideInKg(NAME_None, 100.f);
					StaticMesh->SetSimulatePhysics(true);
					StaticMesh->AddImpulse(DestructibleComponent->CalculateImpulseVector(Frog));
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("Missing static mesh reference!"))
				}
				GetWorld()->GetTimerManager().SetTimer(TimerHandle, DestructibleComponent, &UCustomDestructibleComponent::KillActor, 3.f,
				                                       false);
			}
		}
	}
	return ActualDamage;
}




FTransform ADestructibleObject::GetStartTransform_Implementation()
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
