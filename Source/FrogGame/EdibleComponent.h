// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EdibleComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FROGGAME_API UEdibleComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UEdibleComponent();

	UPROPERTY(EditAnywhere, Category = Edible)
	TSubclassOf<class ASphereDrop> Drop;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Destructible)
	float Health{100.f};

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, SaveGame, Category = Destructible)
	float CurrentHealth;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Destructible)
	float FlyAwayForce{750000.f};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = Edible)
	int32 NumDrops{5};

	// Amount of Points earned towards getting powermode gained when eating this object.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Edible)
	float PowerPoints{0.1f};
	// Amount of Score granted for eating this edible.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Edible)
	float ScorePoints{500.f};
protected:
	// Called when the game starts
	void BeginPlay() override;

public:
	// Called every frame
	void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void SpawnSpheres() const;
	FVector CalculateImpulseVector(class AFrogGameCharacter* Frog) const;
	void KillActor() const;
};
