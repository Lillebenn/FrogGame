// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TongueProjectile.generated.h"

UCLASS()
class FROGGAME_API ATongueProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATongueProjectile();

	/** Sphere collision **/
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Tongue)
	class UCapsuleComponent* BaseCollisionComponent;

	/** StaticMeshComponent to represent the Tongue in the level */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Tongue)
	class UStaticMeshComponent* TongueMesh;

	/** Projectile component **/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Tongue)
	class UProjectileMovementComponent* TongueProjectile;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
