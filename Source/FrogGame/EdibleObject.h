// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DestructibleObject.h"
#include "Edible.h"
#include "EdibleObject.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class FROGGAME_API AEdibleObject : public ADestructibleObject
{
	GENERATED_BODY()


public:
	AEdibleObject();
	void Tick(float DeltaTime) override;


	bool bShouldDestroy{false};

	FTransform StartTransform;

	// Interface stuff
	bool IsDisabled_Implementation() override;


protected:
	// Called when the game starts or when spawned
	void BeginPlay() override;

};
