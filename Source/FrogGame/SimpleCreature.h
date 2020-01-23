// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Edible.h"
#include "EdibleInfo.h"
#include "SimpleCreature.generated.h"

UCLASS(Abstract)
class FROGGAME_API ASimpleCreature : public APawn, public IEdible
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Edible)
	FEdibleInfo EdibleInfo;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void Consume(float FrogSize, const FString& BoneName) override;

	virtual float GetSize() const override;

};
