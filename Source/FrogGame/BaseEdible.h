// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DestructibleActor.h"
#include "EdibleInfo.h"
#include "Edible.h"
#include "Saveable.h"
#include "BaseEdible.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class FROGGAME_API ABaseEdible : public ADestructibleActor, public IEdible, public ISaveable
{
	GENERATED_BODY()

public:
	ABaseEdible();
	void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = Edible)
	FEdibleInfo EdibleInfo;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Edible)
	TArray<class UStaticMesh*> StaticMeshes;
	// Interface stuff
	FEdibleInfo GetInfo_Implementation() const override;
	void DisableActor_Implementation() override;
	USceneComponent* GetTargetComponent_Implementation() override;
	void ActorSaveDataSaved_Implementation() override;
	void ActorSaveDataLoaded_Implementation() override;

	FTransform GetStartTransform() override;


protected:
	// Called when the game starts or when spawned
	void BeginPlay() override;
};
