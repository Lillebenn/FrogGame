// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseEdible.h"
#include "BaseDestructible.generated.h"

/**
 * 
 */
UCLASS()
class FROGGAME_API ABaseDestructible : public ADestructibleActor, public IEdible, public ISaveable
{
	GENERATED_BODY()
public:
	ABaseDestructible();
	void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = Edible)
	FEdibleInfo EdibleInfo;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Edible)
	TArray<class UStaticMesh*> StaticMeshes;
	// Interface stuff
	void DisableActor_Implementation() override;
	FEdibleInfo GetInfo_Implementation() const;
	USceneComponent* GetTargetComponent_Implementation() override;
	void ActorSaveDataSaved_Implementation() override;
	void ActorSaveDataLoaded_Implementation() override;

	FTransform GetStartTransform() override;

	float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

protected:
	// Called when the game starts or when spawned
	void BeginPlay() override;

	float Health;


};
