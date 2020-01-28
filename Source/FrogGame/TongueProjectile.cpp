// Fill out your copyright notice in the Description page of Project Settings.


#include "TongueProjectile.h"
#include "FrogGameCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/TimelineComponent.h"
#include "Engine.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Edible.h"

// Sets default values
ATongueProjectile::ATongueProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create the root SphereComponent to handle the Tongue's collision
	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	CollisionSphere->SetSphereRadius(1);
	CollisionSphere->OnComponentHit.AddDynamic(this, &ATongueProjectile::OnComponentHit);
	// Set the SphereComponent as the root component.
	RootComponent = CollisionSphere;

	//Create the static mesh component 
	TongueMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TongueMesh"));
	TongueMesh->SetSimulatePhysics(false);
	TongueMesh->SetupAttachment(RootComponent);

	// Creates the projectileMovement component and set some defaults.
	TongueProjectile = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("TongueProjectile"));
	TongueProjectile->InitialSpeed = 5000;
	TongueProjectile->MaxSpeed = 5000;
	TongueProjectile->Velocity.X = 5000;
}

void ATongueProjectile::TimelineProgress(float Value)
{
	FVector NewLoc = FMath::Lerp(StartLoc, EndLoc, Value);
	SetActorLocation(NewLoc);
}

void ATongueProjectile::LerpMoveActor(AActor* MovedActor, const float InAlpha)
{
	MovedActor->SetActorLocation(FMath::Lerp(MovedActor->GetActorLocation(), OriginVector, InAlpha));
}

void ATongueProjectile::OnComponentHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                       FVector NormalImpulse, const FHitResult& Hit)
{
	StartDragObjectTimeline();
	if (OtherActor->Implements<UEdible>())
	{
		AFrogGameCharacter* Froggy{Cast<AFrogGameCharacter>(GetParentActor())};
		if (Froggy)
		{
			Target = OtherActor;
			FAttachmentTransformRules InRule(EAttachmentRule::KeepWorld, false);
			Target->AttachToActor(this, InRule);
			Target->SetActorEnableCollision(false);
			// Turn off collision on the dragged object so we don't get affected by it on the way back.
			IEdible::Execute_Consume(Target, Froggy->GetActorScale().Size(), "");
			// Runs the object's custom consume function.
		}
	}
}

// Called when the game starts or when spawned
void ATongueProjectile::BeginPlay()
{
	Super::BeginPlay();

	OriginVector = GetActorLocation();
	FTimerDelegate TimerDel;
	FTimerHandle TimerHandle;
	TimerDel.BindUFunction(this, FName("StartReturnTimeline"));

	GetWorldTimerManager().SetTimer(TimerHandle, TimerDel, 0.1f, false);
}

// Called every frame
void ATongueProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
