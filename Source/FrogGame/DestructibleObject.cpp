// Fill out your copyright notice in the Description page of Project Settings.


#include "DestructibleObject.h"

#include "CustomDestructibleComponent.h"
#include "Edible.h"
#include "FrogGameCharacter.h"
#include "FrogGameInstance.h"
#include "Engine/StaticMesh.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ADestructibleObject::ADestructibleObject()
{
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetCollisionProfileName(TEXT("Edible"));
	RootComponent = StaticMesh;
	DestructibleComponent = CreateDefaultSubobject<UCustomDestructibleComponent>(TEXT("Destructible"));
	EdibleComponent = CreateDefaultSubobject<UEdibleComponent>(TEXT("Edible Info"));
	StaticMesh->SetReceivesDecals(false);
	StaticMesh->SetMobility(EComponentMobility::Static);

	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

void ADestructibleObject::PlayHitSound() const
{
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), FFrogLibrary::GetRandomSoundByArray(HitSounds),
	                                      GetActorLocation(), FRotator());
}

void ADestructibleObject::SetActive_Implementation(bool bActive)
{
	if(bIsActive == bActive)
	{
		return;
	}
	bIsActive = bActive;
	SetActorTickEnabled(bIsActive);
	//SetActorEnableCollision(bActive); // Tried this, it seems too expensive for this purpose
}

void ADestructibleObject::SetMobility_Implementation(bool bShouldActivate)
{
	if (bShouldActivate)
	{
		StaticMesh->SetMobility(EComponentMobility::Movable);
		EndCullingEvent();
	}
	else
	{
		CullingEvent();
		if(!StaticMesh->IsSimulatingPhysics())
		{
			StaticMesh->SetMobility(EComponentMobility::Static);
		}
	}
}

// Called when the game starts or when spawned
void ADestructibleObject::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ADestructibleObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

float ADestructibleObject::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
                                      AActor* DamageCauser)
{
	const float ActualDamage{Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser)};
	//UE_LOG(LogTemp, Warning, TEXT("Taking %f damage."), ActualDamage);
	if (ActualDamage > 0.f)
	{
		float& CurrentHealth{DestructibleComponent->CurrentHealth};
		CurrentHealth -= ActualDamage;
		if (CurrentHealth <= 0.f)
		{
			DeathDamage = ActualDamage;
			if (StaticMesh)
			{
				ActivatePhysics();
				ImpulseVector = DestructibleComponent->CalculateImpulseVector(DamageCauser);
				StaticMesh->AddImpulse(ImpulseVector);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Missing static mesh reference!"))
			}
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, DestructibleComponent,
			                                       &UCustomDestructibleComponent::KillActor, 1.f,
			                                       false);
		}
	}
	return ActualDamage;
}

void ADestructibleObject::ActivatePhysics()
{
	StaticMesh->SetMassOverrideInKg(NAME_None, 100.f);
	StaticMesh->SetSimulatePhysics(true);
	StaticMesh->SetNotifyRigidBodyCollision(true);
	StaticMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	StaticMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
}

void ADestructibleObject::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor != nullptr && OtherActor != this && OtherComp != nullptr)
	{
		UCustomDestructibleComponent* OtherDestructible = Cast<UCustomDestructibleComponent>(
			OtherActor->GetComponentByClass(UCustomDestructibleComponent::StaticClass()));
		if (OtherDestructible && DestructibleComponent->FlyAwayForce > 50000.f)
		{
			UE_LOG(LogTemp, Warning, TEXT("%f"), DestructibleComponent->FlyAwayForce);
			OtherDestructible->FlyAwayForce = DestructibleComponent->FlyAwayForce / 2.f;
			OtherActor->TakeDamage(DeathDamage, FDamageEvent(), GetInstigatorController(), this);
		}
	}
}
