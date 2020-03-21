// Fill out your copyright notice in the Description page of Project Settings.

#include "EdibleObject.h"
#include "Engine/StaticMesh.h"
#include "FrogGameCharacter.h"
#include "SphereDrop.h"
#include "EdibleComponent.h"
#include "Kismet/GameplayStatics.h"


AEdibleObject::AEdibleObject()
{
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetCollisionObjectType(ECC_GameTraceChannel1);
	EdibleComponent = CreateDefaultSubobject<UEdibleComponent>(TEXT("Edible Info"));

	RootComponent = StaticMesh;
}

void AEdibleObject::Tick(float DeltaTime)
{
}


bool AEdibleObject::IsDisabled_Implementation()
{
	return ShouldDestroy;
}

// Custom behaviour when saving or loading
void AEdibleObject::ActorSaveDataSaved_Implementation()
{
}

void AEdibleObject::ActorSaveDataLoaded_Implementation()
{
}

FTransform AEdibleObject::GetStartTransform()
{
	return GetTransform();
}



void AEdibleObject::BeginPlay()
{
	Super::BeginPlay();
}

float AEdibleObject::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
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
			EdibleComponent->CurrentHealth -= ActualDamage;
			if (EdibleComponent->CurrentHealth <= 0.f && !IsActorBeingDestroyed() && !ShouldDestroy)
			{
				// TODO: Maybe set mass to 100kg once it loses all health, so it flies away only when punched to death

				StaticMesh->SetSimulatePhysics(true);
				StaticMesh->AddImpulse(EdibleComponent->CalculateImpulseVector(Frog));
				ShouldDestroy = true;
				GetWorld()->GetTimerManager().SetTimer(TimerHandle, EdibleComponent, &UEdibleComponent::KillActor, 1.f,
				                                       false);
			}
		}
	}
	return ActualDamage;
}
