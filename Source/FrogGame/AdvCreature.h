// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Edible.h"
#include "Saveable.h"
#include "AdvCreature.generated.h"

/**
 * Creature with the ability to use animations and more advanced AI.
 */
UCLASS(Abstract)
class FROGGAME_API AAdvCreature : public ACharacter, public IEdible, public ISaveable
{
	GENERATED_BODY()


public:
	// Sets default values for this character's properties
	AAdvCreature();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UEdibleComponent* EdibleComponent;
	// Called every frame
	void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	bool IsDisabled_Implementation() override;
	bool ShouldDestroy{false};


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Edible)
	FName AttachBoneName{TEXT("None")};

	void DisableActor_Implementation() override;
	void OnDisabled_Implementation() override;


	void ActorSaveDataSaved_Implementation() override;
	void CalculateBoundingSize();
	void ActorSaveDataLoaded_Implementation() override;

	FTransform GetStartTransform() override;
protected:
	// Called when the game starts or when spawned
	void BeginPlay() override;

private:
	FTransform StartTransform;
};
