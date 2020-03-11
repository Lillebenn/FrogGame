// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DestructibleActor.h"
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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UTonguePivot* TongueTarget;

public:
	AEdibleObject();
	void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = AutoAim)
	class UTargetingReticule* Reticule;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = Edible)
	int32 NumDrops{5};
		UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = Edible)
	FEdibleInfo EdibleInfo;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Destructible)
	float Health{100.f};

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, SaveGame, Category = Destructible)
	float CurrentHealth;

	FTimerHandle TimerHandle;
	void KillActor();

	UPROPERTY(EditDefaultsOnly, Category = Edible)
	TSubclassOf<class ASphereDrop> Drop;
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

private:
	void SpawnSpheres() const;
};
