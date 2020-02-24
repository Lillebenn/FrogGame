// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TimelineComponent.h"
#include "Materials/Material.h"
#include "TongueProjectile.generated.h"

class UCurveFloat;


UCLASS()
class FROGGAME_API ATongueProjectile : public AActor
{
	GENERATED_BODY()

	friend class AFrogGameCharacter;
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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Cable)
	UMaterial* CableMaterial;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UCameraShake> TongueShakeEffect;
	// Speed at which the tongue snaps out at a target. Default value - Will be overridden by FrogCharacter.
	float TongueReturnSpeed{4500.0f};
	// Speed at which the tongue returns. Default value - Will be overridden by FrogCharacter.
	float TongueSeekSpeed{10000.0f};
	// Duration at which the tongue will "pause" at the target before snapping back in.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Tongue)
	float PauseDuration{0.1f};
	float CurrentPause;
	bool bIsPaused;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tongue)
	float TongueRange{700.0f};
	void VInterpTo(FVector InterpTo, float TongueSpeed, float DeltaTime);

	void AttachEdible(AActor* EdibleActor);

	void ActivateTongue(AActor* InTarget);

	UPROPERTY(BlueprintReadOnly)
	AActor* Target;
	UPROPERTY()
	class AFrogGameCharacter* Froggy;

	bool bShouldReturn{false};
	FVector TargetLocation;

	UFUNCTION()
	void OnComponentHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                    FVector NormalImpulse, const FHitResult& Hit);


private:
	void SeekTarget(float DeltaTime);
	void Return(float DeltaTime);
	bool Activated{false};
	UPROPERTY()
	class UCableComponent* Cable;

protected:
	// Called when the game starts or when spawned
	void BeginPlay() override;


public:
	// Called every frame
	void Tick(float DeltaTime) override;
};
