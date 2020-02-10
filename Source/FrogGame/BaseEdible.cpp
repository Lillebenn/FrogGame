// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseEdible.h"
#include "DestructibleComponent.h"
#include "DestructibleFractureSettings.h"
#include "Engine/StaticMesh.h"
#include "Components/StaticMeshComponent.h"
#include "DestructibleMesh.h"

ABaseEdible::ABaseEdible()
{
	if (GetDestructibleComponent())
	{
		GetDestructibleComponent()->SetCollisionObjectType(ECC_GameTraceChannel1);
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
	return GetDestructibleComponent();
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

	if (GetDestructibleComponent())
	{
		const FVector RoughSize = GetDestructibleComponent()->GetDestructibleMesh()->GetBounds().GetBox().GetSize();
		const FVector AbsoluteSize{RoughSize.GetAbs()};
		// Get the average axis value of the bounding box
		EdibleInfo.Size = (AbsoluteSize.X + AbsoluteSize.Y + AbsoluteSize.Z) / 3;
		UDestructibleMesh* DestructibleMesh{GetDestructibleComponent()->GetDestructibleMesh()};
		if (DestructibleMesh)
		{
			FDestructibleSpecialHierarchyDepths Hierarchy;
			Hierarchy.bEnableDebris = false;
			Hierarchy.SupportDepth = 1;
			DestructibleMesh->DefaultDestructibleParameters.SpecialHierarchyDepths = Hierarchy;
			DestructibleMesh->FractureSettings->CellSiteCount = EdibleInfo.NumChunks;
		}
		// Temp for testing
		GetDestructibleComponent()->ApplyDamage(1000, GetActorLocation(), GetActorLocation(), 100);

		//Destroy();
	}
}

