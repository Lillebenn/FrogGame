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
	UPROPERTY(EditAnywhere, Category = "Collision")
	class UCapsuleComponent* CapsuleComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* CreatureMesh;

public:
	// Sets default values for this pawn's properties
	ASimpleCreature();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UEdibleComponent* EdibleComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UCustomDestructibleComponent* DestructibleComponent;
	UPROPERTY(BlueprintReadOnly)
	class UFloatingPawnMovement* MovementComponent;
	UStaticMeshComponent* GetMesh();

	void DisableActor_Implementation() override;
	void PauseAI_Implementation(bool bPause = true) override;
	void OnDisabled_Implementation() override;
	bool IsDisabled_Implementation() override;

	void ActorSaveDataLoaded_Implementation() override;
	void ActorSaveDataSaved_Implementation() override;


	FTransform GetStartTransform_Implementation() override;
	bool bShouldDestroy{false};

	FTimerHandle TimerHandle;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	int MoveDistance{500};

	;

	float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	                 AActor* DamageCauser) override;
protected:
	// Called when the game starts or when spawned
	void BeginPlay() override;
private:
	UPROPERTY()
	FTransform StartTransform;
public:
	// Called every frame
	void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
