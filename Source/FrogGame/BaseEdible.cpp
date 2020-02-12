// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseEdible.h"
#include "DestructibleComponent.h"
#include "DestructibleFractureSettings.h"
#include "Engine/StaticMesh.h"
#include "DestructibleMesh.h"

ABaseEdible::ABaseEdible()
{
	if (GetDestructibleComponent())
	{
		GetDestructibleComponent()->SetCollisionObjectType(ECC_GameTraceChannel1);
	}
}

void ABaseEdible::Tick(float DeltaTime)
{
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

	UDestructibleComponent* Destructible{GetDestructibleComponent()};
	if (Destructible)
	{
		if (StaticMeshes.Num() > 0)
		{
			UStaticMesh* Mesh{StaticMeshes[FMath::RandRange(0, StaticMeshes.Num() - 1)]};
			UDestructibleMesh* DestructibleMesh{NewObject<UDestructibleMesh>(this, UDestructibleMesh::StaticClass())};
			DestructibleMesh->BuildFromStaticMesh(*Mesh);
			DestructibleMesh->CreateFractureSettings();
			Destructible->SetDestructibleMesh(DestructibleMesh);
		}
		const FVector RoughSize = Destructible->GetDestructibleMesh()->GetBounds().GetBox().GetSize();
		const FVector AbsoluteSize{RoughSize.GetAbs()};
		// Get the average axis value of the bounding box
		EdibleInfo.Size = (AbsoluteSize.X + AbsoluteSize.Y + AbsoluteSize.Z) / 3;
		UDestructibleMesh* DestructibleMesh{Destructible->GetDestructibleMesh()};
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
