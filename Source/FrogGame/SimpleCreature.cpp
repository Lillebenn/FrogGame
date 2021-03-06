// Fill out your copyright notice in the Description page of Project Settings.


#include "SimpleCreature.h"
#include "AIController.h"
#include "BrainComponent.h"
#include "Components/StaticMeshComponent.h"
#include "CustomDestructibleComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "FrogGameInstance.h"
#include "FrogGameCharacter.h"
#include "EdibleComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ASimpleCreature::ASimpleCreature()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Component"));
	CapsuleComponent->SetCollisionProfileName(TEXT("Edible"));
	RootComponent = CapsuleComponent;

	CreatureMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Creature Mesh"));
	CreatureMesh->SetReceivesDecals(false);
	CreatureMesh->SetCollisionProfileName(TEXT("NoCollision"));
	CreatureMesh->SetupAttachment(RootComponent);
	EdibleComponent = CreateDefaultSubobject<UEdibleComponent>(TEXT("Edible Info"));
	EdibleComponent->bAllowSuction = true;

	DestructibleComponent = CreateDefaultSubobject<UCustomDestructibleComponent>(TEXT("Destructible"));
	MovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MovementComponent"));


	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}


// Called when the game starts or when spawned
void ASimpleCreature::BeginPlay()
{
	Super::BeginPlay();
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

bool ASimpleCreature::IsDisabled_Implementation()
{
	return bShouldDestroy;
}


void ASimpleCreature::SetActive_Implementation(const bool bActive)
{
	bIsIdle = !bActive;
	SetActorTickEnabled(bActive);

	if (bActive)
	{
		StartActing();
	}
	else
	{
		StopAllActions();
	}
}

void ASimpleCreature::PlayHitSound_Implementation() const
{
	if (bShouldSqueak)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), GetSqueakSound(), GetActorLocation(), FRotator());
	}
}

void ASimpleCreature::DisableActor_Implementation()
{
	// Not 100% sure if this is necessary, but we don't need the AI to keep running after being snatched.
	AController* AI{GetController()};
	if (AI)
	{
		AI->StopMovement();
		AI->Destroy();
	}

	bShouldDestroy = true;
}

void ASimpleCreature::PauseAI_Implementation(bool bPause)
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

UStaticMeshComponent* ASimpleCreature::GetMesh()
{
	return CreatureMesh;
}


void ASimpleCreature::ActivatePhysics() const
{
	CapsuleComponent->SetMassOverrideInKg(NAME_None, 100.f);
	CapsuleComponent->SetSimulatePhysics(true);
	CapsuleComponent->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	CapsuleComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
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
			float& CurrentHealth{DestructibleComponent->CurrentHealth};
			CurrentHealth -= ActualDamage;
			if (CurrentHealth <= 0.f && !IsActorBeingDestroyed())
			{
				DisableActor_Implementation();
				// TODO: Maybe set mass to 2-300kg once it loses all health, so it flies away at a decent rate
				ActivatePhysics();
				CapsuleComponent->AddImpulse(DestructibleComponent->CalculateImpulseVector(Frog));
				GetWorld()->GetTimerManager().SetTimer(TimerHandle, DestructibleComponent,
				                                       &UCustomDestructibleComponent::KillActor, 1.f,
				                                       false);
			}
		}
	}
	return ActualDamage;
}
