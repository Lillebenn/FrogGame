// Fill out your copyright notice in the Description page of Project Settings.


#include "SimpleCreature.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/Controller.h"
#include "FrogGameInstance.h"
#include "FrogGameCharacter.h"
#include "EdibleComponent.h"


// Sets default values
ASimpleCreature::ASimpleCreature()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CreatureMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Creature Mesh"));
	CreatureMesh->SetCollisionProfileName(TEXT("EdibleProfile"));

	RootComponent = CreatureMesh;
	EdibleComponent = CreateDefaultSubobject<UEdibleComponent>(TEXT("Edible Info"));
	EdibleComponent->NumDrops = 0;
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


void ASimpleCreature::DisableActor()
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


void ASimpleCreature::OnDisabled_Implementation()
{
	DisableActor();
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
				DisableActor();
				// TODO: Maybe set mass to 2-300kg once it loses all health, so it flies away at a decent rate
				CreatureMesh->SetSimulatePhysics(true);
				CreatureMesh->AddImpulse(EdibleComponent->CalculateImpulseVector(Frog));
				GetWorld()->GetTimerManager().SetTimer(TimerHandle, EdibleComponent, &UEdibleComponent::KillActor, 1.f,
				                                       false);
			}
		}
	}
	return ActualDamage;
}
