// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseEdible.h"
#include "Engine/StaticMesh.h"
#include "FrogGameCharacter.h"
#include "SphereDrop.h"
#include "TargetingReticule.h"
#include "Kismet/GameplayStatics.h"
#include "TonguePivot.h"

ABaseEdible::ABaseEdible()
{
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetCollisionObjectType(ECC_GameTraceChannel1);
	StaticMesh->SetSimulatePhysics(true);
	RootComponent = StaticMesh;
	TongueTarget = CreateDefaultSubobject<UTonguePivot>(TEXT("Tongue Pivot Object"));
	TongueTarget->SetupAttachment(RootComponent);
	Reticule = CreateDefaultSubobject<UTargetingReticule>(TEXT("Targeting Reticule"));
	Reticule->SetupAttachment(RootComponent);
}

void ABaseEdible::Tick(float DeltaTime)
{
}

UTargetingReticule* ABaseEdible::GetTargetingReticule_Implementation()
{
	return Reticule;
}

void ABaseEdible::KillActor()
{
	SpawnSpheres();
	SetLifeSpan(0.001f);
}

FEdibleInfo ABaseEdible::GetInfo_Implementation() const
{
	return EdibleInfo;
}

void ABaseEdible::DisableActor_Implementation()
{
}

UTonguePivot* ABaseEdible::GetTargetComponent_Implementation()
{
	return TongueTarget;
}

// Custom behaviour when saving or loading
void ABaseEdible::ActorSaveDataSaved_Implementation()
{
}

void ABaseEdible::ActorSaveDataLoaded_Implementation()
{
}

FTransform ABaseEdible::GetStartTransform()
{
	return GetTransform();
}

void ABaseEdible::CalculateSize()
{
	if (StaticMesh)
	{
		if (StaticMesh->GetStaticMesh())
		{
			const FVector RoughSize = StaticMesh->GetStaticMesh()->GetBoundingBox().GetSize();
			const FVector AbsoluteSize{RoughSize.GetAbsMin()};
			// Get the average axis value of the bounding box
			EdibleInfo.Size = (AbsoluteSize.X + AbsoluteSize.Y + AbsoluteSize.Z) / 6;
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("%s is missing a static mesh!"), *GetName())
		}
	}
	if (!EdibleInfo.bAutomaticSizeTier)
	{
		EdibleInfo.SizeTier = IEdible::CalculateSizeTier(EdibleInfo.Size);
	}
}

float ABaseEdible::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
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
			if (CurrentHealth <= 0.f && !IsActorBeingDestroyed())
			{
				// TODO: Maybe set mass to 100kg once it loses all health, so it flies away only when punched to death
				FVector ImpulseDirection{Frog->GetActorLocation() - GetActorLocation()};
				ImpulseDirection.Normalize();
				const FVector Impulse{ImpulseDirection * 750000.f};
				StaticMesh->AddImpulse(Impulse);
				GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ABaseEdible::KillActor, 1.f,
				                                       false);
			}
		}
	}
	return ActualDamage;
}

void ABaseEdible::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = Health;
	CalculateSize();
	if (Reticule)
	{
		Reticule->InitWidget();
	}
}

void ABaseEdible::SpawnSpheres() const
{
	if (Drop)
	{
		const float SphereSize{EdibleInfo.Size / NumDrops};
		for (int i{0}; i < 5; i++)
		{
			const FVector2D SpawnLocation2D{FMath::RandPointInCircle(125.f)};
			const FVector SpawnLocation{
				GetActorLocation().X + SpawnLocation2D.X, GetActorLocation().Y + SpawnLocation2D.Y, GetActorLocation().Z
			};
			const FTransform SpawnTransform{SpawnLocation};
			ASphereDrop* Sphere{GetWorld()->SpawnActorDeferred<ASphereDrop>(Drop, SpawnTransform)};
			Sphere->EdibleInfo = EdibleInfo;
			Sphere->EdibleInfo.Size = SphereSize;
			UGameplayStatics::FinishSpawningActor(Sphere, SpawnTransform);
		}
	}
}
