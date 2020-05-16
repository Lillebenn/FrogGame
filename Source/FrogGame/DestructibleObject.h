// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Edible.h"
#include "GameFramework/Actor.h"
#include "FrogGameCharacter.h"
#include "DestructibleObject.generated.h"

UCLASS()
class FROGGAME_API ADestructibleObject : public AActor, public IEdible
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
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Sound)
	USoundConcurrency* ConcurrencySettings;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Sound)
	TArray<class USoundCue*> HitSounds;
	void PlayHitSound_Implementation() const;

	UFUNCTION(BlueprintImplementableEvent)
	void CullingEvent();
	UFUNCTION(BlueprintImplementableEvent)
	void EndCullingEvent();

	void SetActive_Implementation(bool bActive) override;

	bool IsActive_Implementation() override
	{
		return bIsActive;
	}
	void SetMobility_Implementation(bool bShouldActivate) override;

protected:
	// Called when the game starts or when spawned
	void BeginPlay() override;
private:

	FTimerHandle TimerHandle;
	FTransform StartTransform;
	FVector ImpulseVector;
	float DeathDamage{500.f};
	bool bIsActive{true};
public:
	// Called every frame
	void Tick(float DeltaTime) override;
	float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	                 AActor* DamageCauser) override;
	void ActivatePhysics();
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse,
	           const FHitResult& Hit);
};
