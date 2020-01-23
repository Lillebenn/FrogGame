// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseEdible.h"
#include "DestructibleComponent.h"
#include "DestructibleMesh.h"

ABaseEdible::ABaseEdible()
{
	// Not sure if this is correct but should in theory get the box size of the mesh to use as a rough size estimate.
	EdibleInfo.RoughSize = GetDestructibleComponent()->GetDestructibleMesh()->GetBounds().BoxExtent;
}

void ABaseEdible::Consume(float FrogSize, const FString& BoneName)
{
}

FVector ABaseEdible::GetSize() const
{
	return EdibleInfo.RoughSize;
}
