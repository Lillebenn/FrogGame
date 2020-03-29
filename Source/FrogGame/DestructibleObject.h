// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FrogGameCharacter.h"
#include "Saveable.h"
#include "DestructibleObject.generated.h"

UCLASS()
class FROGGAME_API ADestructibleObject : public AActor, public ISaveable
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ADestructibleObject();
	UPROPERTY(EditAnywhere, Category = Destructible)
	TSubclassOf<class ASphereDrop> Drop;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Destructible)
	float Health{100.f};

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, SaveGame, Category = Destructible)
	float CurrentHealth;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Destructible)
	float FlyAwayForce{750000.f};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = Destructible)
	int32 NumDrops{5};
	bool ShouldDestroy{false};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UStaticMeshComponent* StaticMesh;
protected:
	// Called when the game starts or when spawned
	void BeginPlay() override;
private:

	FTimerHandle TimerHandle;
	FTransform StartTransform;
public:
	// Called every frame
	void Tick(float DeltaTime) override;
	float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	                 AActor* DamageCauser) override;
	void SpawnSpheres() const;
	FVector CalculateImpulseVector(AFrogGameCharacter* Frog) const;
	void KillActor();
	void ActorSaveDataSaved_Implementation() override;
	void ActorSaveDataLoaded_Implementation() override;

	FTransform GetStartTransform() override;
};
