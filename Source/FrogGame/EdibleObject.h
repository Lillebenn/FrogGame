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

	// Interface stuff
	bool IsDisabled_Implementation() override;
	bool bShouldDestroy{false};
	void ActorSaveDataSaved_Implementation() override;
	void ActorSaveDataLoaded_Implementation() override;

	FTransform GetStartTransform() override;


protected:
	// Called when the game starts or when spawned
	void BeginPlay() override;
private:
	FTransform StartTransform;
};
