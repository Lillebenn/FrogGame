// Fill out your copyright notice in the Description page of Project Settings.


#include "FlyingSimpleCreature.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "CustomDestructibleComponent.h"
#include "FrogGameCharacter.h"


AFlyingSimpleCreature::AFlyingSimpleCreature()
{
	Nav = CreateDefaultSubobject<USphereComponent>(TEXT("NavCollider"));
	Nav->InitSphereRadius(5.f);
	Nav->SetCollisionProfileName(TEXT("NoCollision"));
	Nav->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Nav->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	Nav->SetupAttachment(RootComponent);
	Nav->bEditableWhenInherited = true;
}

void AFlyingSimpleCreature::BeginPlay()
{
	Super::BeginPlay();
	Nav->SetRelativeLocation(FVector(
		0.f, 0.f, -(Nav->GetScaledSphereRadius() + CapsuleComponent->GetScaledCapsuleHalfHeight())));
}

float AFlyingSimpleCreature::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
                                        AController* EventInstigator,
                                        AActor* DamageCauser)
{
	const float ActualDamage{Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser)};
	UE_LOG(LogTemp, Warning, TEXT("Taking %f damage."), ActualDamage);
	if (ActualDamage > 0.f)
	{
		AFrogGameCharacter* Frog{Cast<AFrogGameCharacter>(DamageCauser)};
		if (Frog)
		{
			float& CurrentHealth{DestructibleComponent->CurrentHealth};
			CurrentHealth -= ActualDamage;
			if (CurrentHealth <= 0.f && !IsActorBeingDestroyed())
			{
				DisableActor_Implementation();
				// TODO: Maybe set mass to 2-300kg once it loses all health, so it flies away only when punched to death
				Nav->SetCollisionProfileName(TEXT("BlockAllDynamic"));
				Nav->SetCollisionObjectType(ECC_GameTraceChannel1);
				Nav->SetSimulatePhysics(true);
				Nav->AddImpulse(DestructibleComponent->CalculateImpulseVector(Frog));
				GetWorld()->GetTimerManager().SetTimer(TimerHandle, DestructibleComponent,
				                                       &UCustomDestructibleComponent::KillActor, 1.f,
				                                       false);
			}
		}
	}
	return ActualDamage;
}
