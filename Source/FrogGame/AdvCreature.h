// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EdibleInfo.h"
#include "Edible.h"
#include "Saveable.h"
#include "AdvCreature.generated.h"

/**
 * Creature with the ability to use animations and more advanced AI.
 */
UCLASS(Abstract)
class FROGGAME_API AAdvCreature : public ACharacter, public IEdible, ISaveable
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAdvCreature();

protected:
	// Called when the game starts or when spawned
	void BeginPlay() override;

public:
	// Called every frame
	void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Edible)
	FEdibleInfo EdibleInfo;


	FEdibleInfo GetInfo_Implementation() const override;

	void DisableActor_Implementation() override;

	USceneComponent* GetTargetComponent_Implementation() override;

	void ActorSaveDataSaved_Implementation() override;
	void ActorSaveDataLoaded_Implementation() override;
};
