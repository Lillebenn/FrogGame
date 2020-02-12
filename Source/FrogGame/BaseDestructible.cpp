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
		//GetDestructibleComponent()->ApplyDamage(1000, GetActorLocation(), GetActorLocation(), 100);
	}
}


FEdibleInfo ABaseDestructible::GetInfo_Implementation() const
{
	return EdibleInfo;
}

void ABaseDestructible::DisableActor_Implementation()
{
}

USceneComponent* ABaseDestructible::GetTargetComponent_Implementation()
{
	return GetDestructibleComponent();
}

FTransform ABaseDestructible::GetStartTransform()
{
	return GetTransform();
}
