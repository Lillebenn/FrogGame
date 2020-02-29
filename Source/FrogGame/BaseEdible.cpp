// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseEdible.h"
#include "Engine/StaticMesh.h"
#include "SphereDrop.h"

ABaseEdible::ABaseEdible()
{
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetCollisionObjectType(ECC_GameTraceChannel1);
	StaticMesh->OnComponentHit.AddDynamic(this, &ABaseEdible::OnComponentHit);
	RootComponent = StaticMesh;
}

void ABaseEdible::Tick(float DeltaTime)
{
}


void ABaseEdible::OnComponentHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	AFrogGameCharacter* Frog{Cast<AFrogGameCharacter>(OtherActor)}; // This needs to be more specific to the punches or we could just walk it to death
	if(Frog)
	{
		CurrentHealth -= 100.f; // TODO: Remove magic number
		if(CurrentHealth <= 0.f)
		{
			Destroy();
		}
		
	}
}

FEdibleInfo ABaseEdible::GetInfo_Implementation() const
{
	return EdibleInfo;
}

void ABaseEdible::DisableActor_Implementation()
{
}

USceneComponent* ABaseEdible::GetTargetComponent_Implementation()
{
	return StaticMesh;
}

// Custom behaviour when saving or loading
void ABaseEdible::ActorSaveDataSaved_Implementation()
{
}

void ABaseEdible::ActorSaveDataLoaded_Implementation()
{
}

FTransform ABaseEdible::GetStartTransform()
{
	return GetTransform();
}

void ABaseEdible::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = Health;
}

void ABaseEdible::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if(Drop)
	{
		for (int i{0}; i < 5; i++)
		{
			GetWorld()->SpawnActor<ASphereDrop>(Drop, GetActorLocation(), FRotator());
		}
	}
	Super::EndPlay(EndPlayReason);
}
