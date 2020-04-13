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
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, SaveGame, Category = Destructible)
	float CurrentHealth;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Destructible)
	float FlyAwayForce{750000.f};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = Destructible)
	int32 NumDrops{5};
protected:
	// Called when the game starts
	void BeginPlay() override;

public:
	// Called every frame
	void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	FVector CalculateImpulseVector(AActor* Actor) const;
	void SpawnSpheres() const;
	void KillActor();
};
