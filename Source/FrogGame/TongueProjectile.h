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


	
	UFUNCTION(BlueprintImplementableEvent)
	void StartReturnTimeline();
	UFUNCTION(BlueprintImplementableEvent)
	void StartDragObjectTimeline();
	UFUNCTION(BlueprintCallable)
	void LerpMoveActor(AActor* MovedActor, float InAlpha);
	UFUNCTION(BlueprintCallable)
	void ConsumeObject();

	UFUNCTION()
	void OnComponentHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	UPROPERTY()
	FVector StartLoc;
	UPROPERTY()
	FVector EndLoc; 
	UPROPERTY(BlueprintReadWrite, Category = Tongue)
	FVector OriginVector;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadOnly)
	AActor* Target;
};


