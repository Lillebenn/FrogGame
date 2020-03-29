// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DestructibleObject.h"
#include "GameFramework/Actor.h"
#include "Edible.h"
#include "EdibleObject.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class FROGGAME_API AEdibleObject : public ADestructibleObject, public IEdible
{
	GENERATED_BODY()


public:
	AEdibleObject();
	void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UEdibleComponent* EdibleComponent;
	bool bShouldDestroy{false};

	// Interface stuff
	bool IsDisabled_Implementation() override;
	void IgnorePawnCollision_Implementation() override;
	void ActorSaveDataSaved_Implementation() override;
	void ActorSaveDataLoaded_Implementation() override;

	FTransform GetStartTransform() override;


protected:
	// Called when the game starts or when spawned
	void BeginPlay() override;
private:
	FTransform StartTransform;
};
