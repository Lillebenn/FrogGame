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
	EdibleComponent = CreateDefaultSubobject<UEdibleComponent>(TEXT("Edible Info"));
	DestructibleComponent = CreateDefaultSubobject<UCustomDestructibleComponent>(TEXT("Destructible"));
	GetMesh()->SetReceivesDecals(false);
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

// Called when the game starts or when spawned
void AAdvCreature::BeginPlay()
{
	Super::BeginPlay();


	StartTransform = GetTransform();
	GetCapsuleComponent()->SetCollisionObjectType(ECC_GameTraceChannel1);
}

// Called every frame
void AAdvCreature::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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
				GetMesh()->SetSimulatePhysics(true);
				GetMesh()->AddImpulse(DestructibleComponent->CalculateImpulseVector(Frog));
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
	OnDisabled_Implementation();
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

void AAdvCreature::OnDisabled_Implementation()
{
	UFrogGameInstance* FrogInstance{Cast<UFrogGameInstance>(GetWorld()->GetGameInstance())};
	if (FrogInstance)
	{
		FrogInstance->OnActorDestroyed(this);
	}
}


// Custom behaviour when saving or loading
void AAdvCreature::ActorSaveDataLoaded_Implementation()
{
}

FTransform AAdvCreature::GetStartTransform_Implementation()
{
	return StartTransform;
}

void AAdvCreature::ActorSaveDataSaved_Implementation()
{
}
