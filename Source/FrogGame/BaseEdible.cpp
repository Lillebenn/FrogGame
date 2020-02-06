// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseEdible.h"
#include "DestructibleComponent.h"
#include "DestructibleMesh.h"

ABaseEdible::ABaseEdible()
{
	if(GetDestructibleComponent())
	{
		GetDestructibleComponent()->SetCollisionObjectType(ECC_GameTraceChannel1);
	}
}


FEdibleInfo ABaseEdible::GetInfo_Implementation() const
{
	return EdibleInfo;
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

FTransform ABaseEdible::GetStartTransform_Implementation()
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
	}
}
