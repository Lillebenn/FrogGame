// Fill out your copyright notice in the Description page of Project Settings.


#include "TongueProjectile.h"
#include "Components/CapsuleComponent.h"
#include "Engine.h"

// Sets default values
ATongueProjectile::ATongueProjectile() {
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create the root CapsuleComponent to handle the Tongue's collision
	BaseCollisionComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("BaseCapsuleComponent"));

	// Set the SphereComponent as the root component.
	RootComponent = BaseCollisionComponent;

	//Create the static mesh component 
	TongueMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TongueMesh"));
	TongueMesh->SetSimulatePhysics(false);
	TongueMesh->AttachTo(RootComponent);

	// Creates the projectileMovement component and set some defaults.
	TongueProjectile = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("TongueProjectile"));
	TongueProjectile->InitialSpeed = 5000;
	TongueProjectile->MaxSpeed = 5000;
	TongueProjectile->Velocity.X = 5000;
}

// Called when the game starts or when spawned
void ATongueProjectile::BeginPlay() {
Super::BeginPlay();
	
}

// Called every frame
void ATongueProjectile::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

}

