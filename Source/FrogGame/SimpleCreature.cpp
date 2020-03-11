// Fill out your copyright notice in the Description page of Project Settings.


#include "SimpleCreature.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/Controller.h"
#include "FrogGameInstance.h"
#include "FrogGameCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "EdibleComponent.h"
#include "SphereDrop.h"


// Sets default values
ASimpleCreature::ASimpleCreature()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CreatureMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Creature Mesh"));
	CreatureMesh->SetCollisionProfileName(TEXT("EdibleProfile"));
	CreatureMesh->SetNotifyRigidBodyCollision(true);

	RootComponent = CreatureMesh;
	EdibleComponent = CreateDefaultSubobject<UEdibleComponent>(TEXT("Edible Info"));

	MovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MovementComponent"));


	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}


bool ASimpleCreature::IsDisabled_Implementation()
{
	return ShouldDestroy;
}


// Called when the game starts or when spawned
void ASimpleCreature::BeginPlay()
{
	Super::BeginPlay();
	//Reticule->InitWidget();
	CalculateBoundingSize(); //This was causing an error somewhere
	StartTransform = GetTransform();
}

// Called every frame
void ASimpleCreature::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ASimpleCreature::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}


void ASimpleCreature::DisableActor_Implementation()
{
	// Not 100% sure if this is necessary, but we don't need the AI to keep running after being snatched.
	AController* AI{GetController()};
	if (AI)
	{
		AI->StopMovement();
		AI->UnPossess();
		AI->Destroy();
	}
}

UStaticMeshComponent* ASimpleCreature::GetMesh()
{
	return CreatureMesh;
}

UEdibleComponent* ASimpleCreature::GetInfo_Implementation() const
{
	return EdibleComponent;
}

void ASimpleCreature::OnDisabled_Implementation()
{
	DisableActor_Implementation();
	UFrogGameInstance* FrogInstance{Cast<UFrogGameInstance>(GetWorld()->GetGameInstance())};
	if (FrogInstance)
	{
		FrogInstance->OnActorDestroyed(this);
	}
}

// Custom behaviour when saving or loading
void ASimpleCreature::ActorSaveDataLoaded_Implementation()
{
}

void ASimpleCreature::ActorSaveDataSaved_Implementation()
{
}


FTransform ASimpleCreature::GetStartTransform()
{
	return StartTransform;
}

void ASimpleCreature::CalculateBoundingSize()
{
	if (CreatureMesh)
	{
		if (CreatureMesh->GetStaticMesh())
		{
			const FVector RoughSize = CreatureMesh->GetStaticMesh()->GetBoundingBox().GetSize();
			const FVector AbsoluteSize{RoughSize.GetAbsMin()};
			// Get the average axis value of the bounding box - Note: Since we're taking the bounding box size we divide the size twice to account for the extra box size.
			EdibleComponent->Size = (AbsoluteSize.X + AbsoluteSize.Y + AbsoluteSize.Z) / 6;
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("%s is missing a mesh!"), *GetName());
		}
	}
	if (EdibleComponent->bAutomaticSizeTier)
	{
		IEdible::CalculateSizeTier(EdibleComponent);
	}
}

float ASimpleCreature::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
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
			if (EdibleComponent->CurrentHealth <= 0.f && !IsActorBeingDestroyed())
			{
				// TODO: Maybe set mass to 100kg once it loses all health, so it flies away only when punched to death
				FVector ImpulseDirection{Frog->GetActorLocation() - GetActorLocation()};
				ImpulseDirection.Normalize();

				CreatureMesh->SetCollisionProfileName(TEXT("BlockAllDynamic"));
				CreatureMesh->SetCollisionObjectType(ECC_GameTraceChannel1);
				CreatureMesh->SetSimulatePhysics(true);

				const FVector Impulse{ImpulseDirection * 750000.f};
				CreatureMesh->AddImpulse(Impulse);
				GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ASimpleCreature::KillActor, 1.f,
				                                       false);
			}
		}
	}
	return ActualDamage;
}


void ASimpleCreature::SpawnSpheres() const
{
	if (EdibleComponent->Drop)
	{
		const float SphereSize{EdibleComponent->GetSphereSize()};
		for (int i{0}; i < 5; i++)
		{
			const FVector2D SpawnLocation2D{FMath::RandPointInCircle(125.f)};
			const FVector SpawnLocation{
				GetActorLocation().X + SpawnLocation2D.X, GetActorLocation().Y + SpawnLocation2D.Y, GetActorLocation().Z
			};
			const FTransform SpawnTransform{SpawnLocation};
			ASphereDrop* Sphere{GetWorld()->SpawnActorDeferred<ASphereDrop>(EdibleComponent->Drop, SpawnTransform)};
			Sphere->EdibleComponent = EdibleComponent;
			Sphere->EdibleComponent->Size = SphereSize;
			UGameplayStatics::FinishSpawningActor(Sphere, SpawnTransform);
		}
	}
}

void ASimpleCreature::KillActor()
{
	SpawnSpheres();
	SetLifeSpan(0.001f);
}
