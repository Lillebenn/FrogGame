// Fill out your copyright notice in the Description page of Project Settings.


#include "FlyingSimpleCreature.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "FrogGameCharacter.h"


AFlyingSimpleCreature::AFlyingSimpleCreature()
{
	NavCollider = CreateDefaultSubobject<UCapsuleComponent>(TEXT("NavCollider"));
	NavCollider->InitCapsuleSize(15.f, 80.0f);
	NavCollider->SetCollisionProfileName(TEXT("EdibleProfile"));
	RootComponent = NavCollider;
	GetMesh()->SetupAttachment(RootComponent);
}

void AFlyingSimpleCreature::BeginPlay()
{
	Super::BeginPlay();
}
float AFlyingSimpleCreature::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
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
				const FVector Impulse{ImpulseDirection * 750000.f};
				NavCollider->SetCollisionProfileName(TEXT("BlockAllDynamic"));
				NavCollider->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel1);
				NavCollider->SetSimulatePhysics(true);
				NavCollider->AddImpulse(EdibleComponent->CalculateImpulseVector(Frog));
				GetWorld()->GetTimerManager().SetTimer(TimerHandle, EdibleComponent, &UEdibleComponent::KillActor, 1.f,
				                                       false);
			}
		}
	}
	return ActualDamage;
}
