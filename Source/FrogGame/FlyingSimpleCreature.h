// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SimpleCreature.h"
#include "FlyingSimpleCreature.generated.h"

/**
 * 
 */
UCLASS()
class FROGGAME_API AFlyingSimpleCreature : public ASimpleCreature
{
	GENERATED_BODY()
	
	// Collider to maintain proper height for navmesh.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Collision, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* Nav;
public:
	AFlyingSimpleCreature();


protected:
	void BeginPlay() override;
	float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	                 AActor* DamageCauser) override;
};
