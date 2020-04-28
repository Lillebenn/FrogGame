// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AdvCreature.h"
#include "FlyingAdvCreature.generated.h"

/**
 * 
 */
UCLASS()
class FROGGAME_API AFlyingAdvCreature : public AAdvCreature
{
	GENERATED_BODY()
public:
	AFlyingAdvCreature();
	// Collider to maintain proper height for navmesh.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Collision)
	class USphereComponent* NavCollider;
protected:
	void BeginPlay() override;
};
