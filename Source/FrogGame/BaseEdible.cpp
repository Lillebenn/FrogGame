// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseEdible.h"
#include "DestructibleComponent.h"
#include "DestructibleMesh.h"

float ABaseEdible::GetFullSize() const
{
	const FVector BoxSize{GetDestructibleComponent()->GetDestructibleMesh()->GetBounds().GetBox().GetSize()};
	return BoxSize.Size();
}

void ABaseEdible::Consume(float FrogSize, const FString& BoneName)
{
}
