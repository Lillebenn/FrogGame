// Fill out your copyright notice in the Description page of Project Settings.


#include "AdvCreature.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Controller.h"
#include "FrogGameInstance.h"
#include "FrogGameCharacter.h"
#include "EdibleComponent.h"
#include "TimerManager.h"

// Sets default values
AAdvCreature::AAdvCreature()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	EdibleComponent = CreateDefaultSubobject<UEdibleComponent>(TEXT("Edible Info"));
	EdibleComponent->NumDrops = 0;
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
			EdibleComponent->CurrentHealth -= ActualDamage;
			if (EdibleComponent->CurrentHealth <= 0.f && !IsActorBeingDestroyed())
			{
				DisableActor();
				// TODO: Maybe set mass to 2-300kg once it loses all health, so it flies away at a decent rate
				GetMesh()->SetSimulatePhysics(true);
				GetMesh()->AddImpulse(EdibleComponent->CalculateImpulseVector(Frog));
				GetWorld()->GetTimerManager().SetTimer(TimerHandle, EdibleComponent, &UEdibleComponent::KillActor, 1.f,
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
	return ShouldDestroy;
}

void AAdvCreature::DisableActor()
{
	AController* AI{GetController()};
	if (AI)
	{
		AI->StopMovement();
		AI->UnPossess();
		AI->Destroy();
	}
}

void AAdvCreature::OnDisabled_Implementation()
{
	DisableActor();
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

FTransform AAdvCreature::GetStartTransform()
{
	return StartTransform;
}

void AAdvCreature::ActorSaveDataSaved_Implementation()
{
}
