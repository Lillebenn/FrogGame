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
class FROGGAME_API AAdvCreature : public ACharacter, public IEdible, public ISaveable
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAdvCreature();
	// Called every frame
	void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UTonguePivot* TongueTarget;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Edible)
	FEdibleInfo EdibleInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Edible)
	FName AttachBoneName{TEXT("None")};

	FEdibleInfo GetInfo_Implementation() const override;

	void DisableActor_Implementation() override;
	void OnDisabled_Implementation() override;

	UTonguePivot* GetTargetComponent_Implementation() override;
	void ActorSaveDataSaved_Implementation() override;
	void CalculateBoundingSize();
	void ActorSaveDataLoaded_Implementation() override;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = AutoAim)
	class UTargetingReticule* Reticule;
	class UTargetingReticule* GetTargetingReticule_Implementation() override;
	FTransform GetStartTransform() override;
protected:
	// Called when the game starts or when spawned
	void BeginPlay() override;

private:
	FTransform StartTransform;
};
