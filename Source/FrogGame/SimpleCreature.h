// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Edible.h"
#include "EdibleInfo.h"
#include "Saveable.h"

#include "SimpleCreature.generated.h"

UCLASS(Abstract)
class FROGGAME_API ASimpleCreature : public APawn, public IEdible, public ISaveable
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASimpleCreature();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh)
	UStaticMeshComponent* CreatureMesh;

	UPROPERTY(BlueprintReadOnly)
	class UFloatingPawnMovement* MovementComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = AutoAim)
	class UTargetingReticle* Reticle;
	class UTargetingReticle* GetTargetingReticule_Implementation() override;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = Edible)
	FEdibleInfo EdibleInfo;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	int MoveDistance{500};

	UPROPERTY()
	FTransform StartTransform;

protected:
	// Called when the game starts or when spawned
	void BeginPlay() override;

public:
	// Called every frame
	void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UStaticMeshComponent* GetMesh();
	FEdibleInfo GetInfo_Implementation() const override;

	void DisableActor_Implementation() override;

	USceneComponent* GetTargetComponent_Implementation() override;

	void OnDisabled_Implementation() override;

	void ActorSaveDataLoaded_Implementation() override;
	void ActorSaveDataSaved_Implementation() override;

	void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	FTransform GetStartTransform() override;
private:
	void CalculateBoundingSize();
};
