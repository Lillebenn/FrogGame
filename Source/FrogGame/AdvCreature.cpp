// Fill out your copyright notice in the Description page of Project Settings.


#include "AdvCreature.h"
#include "AIController.h"
#include "BrainComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "CustomDestructibleComponent.h"
#include "FrogGameInstance.h"
#include "FrogGameCharacter.h"
#include "EdibleComponent.h"
#include "TimerManager.h"

// Sets default values
AAdvCreature::AAdvCreature()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Edible"));
	EdibleComponent = CreateDefaultSubobject<UEdibleComponent>(TEXT("Edible Info"));
	EdibleComponent->bAllowSuction = true;

	DestructibleComponent = CreateDefaultSubobject<UCustomDestructibleComponent>(TEXT("Destructible"));
	GetMesh()->SetReceivesDecals(false);
	GetMesh()->SetCollisionProfileName(TEXT("NoCollision"));
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

// Called when the game starts or when spawned
void AAdvCreature::BeginPlay()
{
	Super::BeginPlay();

	GetCapsuleComponent()->SetCollisionObjectType(ECC_GameTraceChannel1);
}


// Called every frame
void AAdvCreature::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AAdvCreature::ActivatePhysics() const
{
	GetCapsuleComponent()->SetMassOverrideInKg(NAME_None, 100.f);
	GetCapsuleComponent()->SetSimulatePhysics(true);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
}

float AAdvCreature::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
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
			float& CurrentHealth{DestructibleComponent->CurrentHealth};
			CurrentHealth -= ActualDamage;
			if (CurrentHealth <= 0.f && !IsActorBeingDestroyed())
			{
				DisableActor_Implementation();
				// TODO: Maybe set mass to 2-300kg once it loses all health, so it flies away at a decent rate
				ActivatePhysics();
				GetCapsuleComponent()->AddImpulse(DestructibleComponent->CalculateImpulseVector(Frog));
				GetWorld()->GetTimerManager().SetTimer(TimerHandle, DestructibleComponent,
				                                       &UCustomDestructibleComponent::KillActor, 1.f,
				                                       false);
			}
		}
	}
	return ActualDamage;
}


// Called to bind functionality to input
void AAdvCreature::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

bool AAdvCreature::IsDisabled_Implementation()
{
	return bShouldDestroy;
}

void AAdvCreature::DisableActor_Implementation()
{
	AController* AI{GetController()};
	if (AI)
	{
		AI->StopMovement();
		AI->Destroy();
	}

	bShouldDestroy = true;
}

void AAdvCreature::PauseAI_Implementation(bool bPause)
{
	AAIController* AI{Cast<AAIController>(GetController())};
	if (AI)
	{
		if (bPause)
		{
			AI->BrainComponent->PauseLogic(TEXT("Hit by whirlwind."));
			AI->StopMovement();
		}
		else
		{
			AI->BrainComponent->ResumeLogic(TEXT("No longer hit by whirlwind."));
		}
	}
}

