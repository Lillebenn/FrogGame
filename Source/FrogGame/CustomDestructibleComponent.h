// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CustomDestructibleComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FROGGAME_API UCustomDestructibleComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UCustomDestructibleComponent();
	UPROPERTY(EditAnywhere, Category = Destructible)
	TSubclassOf<class ASphereDrop> Drop;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Destructible)
	float MaxHealth{100.f};
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = Destructible)
	float CurrentHealth;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Destructible)
	float FlyAwayForce{750000.f};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Destructible)
	bool bSpawnSphereDrops{true};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Destructible, meta = (EditCondition = "bSpawnSphereDrops"))
	int32 NumDrops{5};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Destructible)
	class UParticleSystem* DestructionSmoke;
	UPROPERTY(EditDefaultsOnly, Category = Destructible)
	float SmokeScale{5.f};
protected:
	// Called when the game starts
	void BeginPlay() override;

public:
	FVector CalculateImpulseVector(AActor* Actor) const;
	void SpawnSpheres() const;
	void KillActor() const;

private:
	class AFrogGameMode* GameMode;
};
