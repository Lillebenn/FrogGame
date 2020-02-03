// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseEdible.h"
#include "DestructibleComponent.h"
#include "DestructibleMesh.h"

ABaseEdible::ABaseEdible()
{
}


FEdibleInfo ABaseEdible::GetInfo_Implementation() const
{
	return EdibleInfo;
}

void ABaseEdible::BeginPlay()
{
	Super::BeginPlay();

	// Not sure if this is correct but should in theory get the box size of the mesh to use as a rough size estimate.
	if (GetDestructibleComponent())
	{
		const FVector RoughSize = GetDestructibleComponent()->GetDestructibleMesh()->GetBounds().GetBox().GetSize();
		const FVector AbsoluteSize{RoughSize.GetAbs()};
		// Get the average axis value of the bounding box
		EdibleInfo.Size = (AbsoluteSize.X + AbsoluteSize.Y + AbsoluteSize.Z) / 3;
	}
}
