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

	bool ShouldDestroy{false};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UStaticMeshComponent* StaticMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UCustomDestructibleComponent* DestructibleComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UEdibleComponent* EdibleComponent;
protected:
	// Called when the game starts or when spawned
	void BeginPlay() override;
	void Destroyed() override;
private:

	FTimerHandle TimerHandle;
	FTransform StartTransform;
	FVector ImpulseVector;
	float DeathDamage{500.f};
public:
	// Called every frame
	void Tick(float DeltaTime) override;
	float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	                 AActor* DamageCauser) override;
	void ActivatePhysics();
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse,
	           const FHitResult& Hit);
	void ActorSaveDataSaved_Implementation() override;
	void ActorSaveDataLoaded_Implementation() override;
	FTransform GetStartTransform_Implementation() override;
	void ResetTransforms_Implementation() override;
};
