// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Edible.h"
#include "Saveable.h"
#include "SimpleCreature.generated.h"

UCLASS(Abstract)
class FROGGAME_API ASimpleCreature : public APawn, public IEdible, public ISaveable
{
	GENERATED_BODY()
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* CreatureMesh;

public:
	// Sets default values for this pawn's properties
	ASimpleCreature();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UEdibleComponent* EdibleComponent;
	UPROPERTY(BlueprintReadOnly)
	class UFloatingPawnMovement* MovementComponent;

	bool IsDisabled_Implementation() override;
	bool ShouldDestroy{false};


	FTimerHandle TimerHandle;
	void KillActor();
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	int MoveDistance{500};

	UPROPERTY()
	FTransform StartTransform;

	float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	                 AActor* DamageCauser) override;
protected:
	// Called when the game starts or when spawned
	void BeginPlay() override;

	void SpawnSpheres() const;

public:
	// Called every frame
	void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UStaticMeshComponent* GetMesh();
	UEdibleComponent* GetInfo_Implementation() const override;

	void DisableActor_Implementation() override;


	void OnDisabled_Implementation() override;

	void ActorSaveDataLoaded_Implementation() override;
	void ActorSaveDataSaved_Implementation() override;


	FTransform GetStartTransform() override;
private:
	void CalculateBoundingSize();
};
