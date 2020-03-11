// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Edible.h"
#include "Saveable.h"
#include "EdibleObject.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class FROGGAME_API AEdibleObject : public AActor, public IEdible, public ISaveable
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* StaticMesh;


public:
	AEdibleObject();
	void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UEdibleComponent* EdibleComponent;
	UPROPERTY(EditDefaultsOnly, Category = Edible)
	TSubclassOf<class ASphereDrop> Drop;

	FTimerHandle TimerHandle;
	void KillActor();


	// Interface stuff
	void DisableActor_Implementation() override;
	bool IsDisabled_Implementation() override;
	bool ShouldDestroy{false};
	void ActorSaveDataSaved_Implementation() override;
	void ActorSaveDataLoaded_Implementation() override;

	FTransform GetStartTransform() override;
	void CalculateSize();


	float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	                 AActor* DamageCauser) override;

protected:
	// Called when the game starts or when spawned
	void BeginPlay() override;
};