// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TimelineComponent.h"
#include "TongueProjectile.generated.h"

class UCurveFloat;

UCLASS()
class FROGGAME_API ATongueProjectile : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATongueProjectile();


	/** Sphere collision **/
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Tongue)
	class USphereComponent* CollisionSphere;

	/** StaticMeshComponent to represent the Tongue in the level */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Tongue)
	class UStaticMeshComponent* TongueMesh;

	/** Projectile component **/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Tongue)
	class UProjectileMovementComponent* TongueProjectile;

	/** Physics Handle component for grabbing fractured chunks **/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Tongue)
	class UPhysicsHandleComponent* PhysicsHandle;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Cable)
	class UMaterial* CableMaterial;
	// Speed at which the tongue snaps out at a target
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Tongue)
	float TongueOutSpeed{2500.0f};
	// Speed at which the tongue returns.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Tongue)
	float TongueInSpeed{4500.0f};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tongue)
	float TongueRange{700.0f};
	void VInterpTo(FVector InterpTo, float TongueSpeed, float DeltaTime);
	
	void AttachEdible(AActor* EdibleActor);
	void AttachEdible(AActor* EdibleActor, FName BoneName) const;


	UPROPERTY(BlueprintReadOnly)
	AActor* Target;
	UPROPERTY()
	class AFrogGameCharacter* Froggy;

	bool bShouldReturn{false};
	FName BoneTarget;
	FVector TargetLocation;

	UFUNCTION()
	void OnComponentHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                    FVector NormalImpulse, const FHitResult& Hit);
	void AttachToEdible(AActor* OtherActor);

private:
	void SeekTarget(float DeltaTime);
	void Return(float DeltaTime);
protected:
	// Called when the game starts or when spawned
	void BeginPlay() override;


public:
	// Called every frame
	void Tick(float DeltaTime) override;
};
