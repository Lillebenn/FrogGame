// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseDestructible.h"
#include "DestructibleMesh.h"
#include "DestructibleComponent.h"
#include "DestructibleFractureSettings.h"

ABaseDestructible::ABaseDestructible()
{
	if (GetDestructibleComponent())
	{
		GetDestructibleComponent()->SetCollisionObjectType(ECC_GameTraceChannel1);
		GetDestructibleComponent()->SetNotifyRigidBodyCollision(true);
		if (GetDestructibleComponent()->GetDestructibleMesh())
		{
			Health = GetDestructibleComponent()
			         ->GetDestructibleMesh()->DefaultDestructibleParameters.DamageParameters.DamageThreshold;
		}
		else
		{
			Health = 100.f;
		}
	}
}

void ABaseDestructible::Tick(float DeltaTime)
{
}

// Custom behaviour when saving or loading
void ABaseDestructible::ActorSaveDataSaved_Implementation()
{
}

void ABaseDestructible::ActorSaveDataLoaded_Implementation()
{
}

void ABaseDestructible::BeginPlay()
{
	Super::BeginPlay();
	UDestructibleComponent* Destructible{GetDestructibleComponent()};
	if (Destructible)
	{
		const FVector RoughSize = Destructible->GetDestructibleMesh()->GetBounds().GetBox().GetSize();
		const FVector AbsoluteSize{RoughSize.GetAbs()};
		// Get the average axis value of the bounding box
		EdibleInfo.Size = (AbsoluteSize.X + AbsoluteSize.Y + AbsoluteSize.Z) / 3;
		UDestructibleMesh* DestructibleMesh{Destructible->GetDestructibleMesh()};
		if (DestructibleMesh)
		{
			FDestructibleSpecialHierarchyDepths Hierarchy;
			Hierarchy.bEnableDebris = true;
			Hierarchy.SupportDepth = 1;
			DestructibleMesh->DefaultDestructibleParameters.SpecialHierarchyDepths = Hierarchy;
			DestructibleMesh->FractureSettings->CellSiteCount = EdibleInfo.NumChunks;
		}
		// Temp for testing
		//GetDestructibleComponent()->ApplyDamage(1000, GetActorLocation(), GetActorLocation(), 100);
	}
}


void ABaseDestructible::DisableActor_Implementation()
{
}

FEdibleInfo ABaseDestructible::GetInfo_Implementation() const
{
	return EdibleInfo;
}

USceneComponent* ABaseDestructible::GetTargetComponent_Implementation()
{
	return GetDestructibleComponent();
}

FTransform ABaseDestructible::GetStartTransform()
{
	return GetTransform();
}

float ABaseDestructible::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
                                    AActor* DamageCauser)
{
	const float ActualDamage{Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser)};
	UE_LOG(LogTemp, Warning, TEXT("TEST"))
	if (ActualDamage > 0.f)
	{
		GetDestructibleComponent()->ApplyRadiusDamage(ActualDamage, DamageCauser->GetActorLocation(), 90.f, 100.f, true);
		Health -= ActualDamage;
		if (Health <= 0.f)
		{
			//GetDestructibleComponent()->AddRadialImpulse(DamageCauser->GetActorLocation(), 90.f, 100.f, RIF_Constant);
		}
	}
	return ActualDamage;
}
