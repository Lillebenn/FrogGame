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


void AEdibleObject::KillActor()
{
	EdibleComponent->SpawnSpheres();

	SetLifeSpan(0.001f);
}


void AEdibleObject::DisableActor_Implementation()
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

void AEdibleObject::CalculateSize()
{
	if (StaticMesh)
	{
		if (StaticMesh->GetStaticMesh())
		{
			const FVector RoughSize = StaticMesh->GetStaticMesh()->GetBoundingBox().GetSize();
			const FVector AbsoluteSize{RoughSize.GetAbsMin()};
			// Get the average axis value of the bounding box
			EdibleComponent->Size = (AbsoluteSize.X + AbsoluteSize.Y + AbsoluteSize.Z) / 6;
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("%s is missing a static mesh!"), *GetName())
		}
	}
	if (EdibleComponent->bAutomaticSizeTier)
	{
		IEdible::CalculateSizeTier(EdibleComponent);
	}
}

void AEdibleObject::BeginPlay()
{
	Super::BeginPlay();
	CalculateSize();
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
				FVector ImpulseDirection{GetActorLocation() - Frog->GetActorLocation()};
				ImpulseDirection.Normalize();
				const FVector Impulse{ImpulseDirection * 750000.f};
				StaticMesh->SetSimulatePhysics(true);
				StaticMesh->AddImpulse(Impulse);
				ShouldDestroy = true;
				GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AEdibleObject::KillActor, 1.f,
				                                       false);
			}
		}
	}
	return ActualDamage;
}

