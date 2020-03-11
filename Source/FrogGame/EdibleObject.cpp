// Fill out your copyright notice in the Description page of Project Settings.


#include "EdibleObject.h"
#include "Engine/StaticMesh.h"
#include "FrogGameCharacter.h"
#include "SphereDrop.h"
#include "TargetingReticule.h"
#include "Kismet/GameplayStatics.h"
#include "TonguePivot.h"

AEdibleObject::AEdibleObject()
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

void AEdibleObject::Tick(float DeltaTime)
{
}


void AEdibleObject::KillActor()
{
	SpawnSpheres();

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
			EdibleInfo.Size = (AbsoluteSize.X + AbsoluteSize.Y + AbsoluteSize.Z) / 6;
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("%s is missing a static mesh!"), *GetName())
		}
	}
	if (EdibleInfo.bAutomaticSizeTier)
	{
		EdibleInfo.SizeTier = IEdible::CalculateSizeTier(EdibleInfo.Size);
	}
}

void AEdibleObject::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = Health;
	CalculateSize();
	if (Reticule)
	{
		Reticule->InitWidget();
	}
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
			CurrentHealth -= ActualDamage;
			if (CurrentHealth <= 0.f && !IsActorBeingDestroyed())
			{
				// TODO: Maybe set mass to 100kg once it loses all health, so it flies away only when punched to death
				FVector ImpulseDirection{Frog->GetActorLocation() - GetActorLocation()};
				ImpulseDirection.Normalize();
				const FVector Impulse{ImpulseDirection * 750000.f};
				StaticMesh->AddImpulse(Impulse);
				ShouldDestroy = true;
				GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AEdibleObject::KillActor, 1.f,
				                                       false);
			}
		}
	}
	return ActualDamage;
}


void AEdibleObject::SpawnSpheres() const
{
	UE_LOG(LogTemp, Warning, TEXT("Spawning spheres."))
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
