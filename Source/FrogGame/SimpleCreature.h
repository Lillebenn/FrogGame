// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "SimpleCreature.generated.h"

UCLASS(Abstract)
class FROGGAME_API ASimpleCreature : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASimpleCreature();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Mesh)
	UStaticMeshComponent* CreatureMesh;
	// Collider to maintain proper height for navmesh.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Collision)
	class UCapsuleComponent* NavCollider;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Collision)
	class USphereComponent* CreatureCollider;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UFloatingPawnMovement* MovementComponent;
	
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
