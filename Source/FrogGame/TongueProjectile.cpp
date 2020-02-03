// Fill out your copyright notice in the Description page of Project Settings.


#include "TongueProjectile.h"
#include "FrogGameCharacter.h"
#include "Engine.h"
#include "Edible.h"

// Sets default values
ATongueProjectile::ATongueProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create the root SphereComponent to handle the Tongue's collision
	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	CollisionSphere->SetSphereRadius(16.0f);
	CollisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionSphere->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECollisionResponse::ECR_Block);
	CollisionSphere->SetNotifyRigidBodyCollision(true);
	CollisionSphere->OnComponentHit.AddDynamic(this, &ATongueProjectile::OnComponentHit);
	// Set the SphereComponent as the root component.
	RootComponent = CollisionSphere;

	//Create the static mesh component 
	TongueMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TongueMesh"));
	TongueMesh->SetSimulatePhysics(false);
	TongueMesh->SetCollisionProfileName("NoCollision");
	TongueMesh->SetupAttachment(RootComponent);
}


void ATongueProjectile::VInterpTo(const FVector InterpTo, const float DeltaTime)
{
	FHitResult HitResult = FHitResult(ForceInit);
	if (!bShouldReturn)
	{
		SetActorLocation(FMath::VInterpConstantTo(GetActorLocation(),
		                                          InterpTo,
		                                          DeltaTime, TongueSpeed), true, &HitResult);
	}
	else
	{
		SetActorLocation(FMath::VInterpConstantTo(GetActorLocation(),
		                                          InterpTo,
		                                          DeltaTime, TongueSpeed), false, &HitResult);
	}
	if (HitResult.IsValidBlockingHit() && !Target) // I feel like I'm adding a lot of useless shit because I'm tired lol
	{
		AActor* HitActor{HitResult.GetActor()};
		bShouldReturn = true;
	}
}

void ATongueProjectile::AttachEdible(AActor* EdibleActor)
{
	const FAttachmentTransformRules InRule(EAttachmentRule::KeepWorld, false);
	EdibleActor->AttachToActor(this, InRule);
	// Turn off collision on the dragged object so we don't get affected by it on the way back.
	EdibleActor->SetActorEnableCollision(false);
}

void ATongueProjectile::OnComponentHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                       FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor->Implements<UEdible>())
	{
		if(Target)
		{
			AttachEdible(Target);
			bShouldReturn = true;
		}
	}
}

void ATongueProjectile::SeekTarget(const float DeltaTime)
{
	if (Target)
	{
		VInterpTo(Target->GetActorLocation(), DeltaTime);
		if ((Target->GetActorLocation() - GetActorLocation()).Size() <= CollisionSphere->GetScaledSphereRadius())
		{
			bShouldReturn = true;
		}
	}
	else
	{
		VInterpTo(TargetLocation, DeltaTime); // Keep going until you hit something or reach the max distance
		if ((TargetLocation - GetActorLocation()).Size() <= 10.f)
		{
			bShouldReturn = true;
		}
	}
}

void ATongueProjectile::Return(const float DeltaTime)
{
	if (Froggy)
	{
		const FVector ReturnPos{Froggy->GetRayMesh()->GetComponentLocation()};
		VInterpTo(ReturnPos, DeltaTime);
		if ((GetActorLocation() - ReturnPos).Size() <= CollisionSphere->GetScaledSphereRadius())
		{
			Froggy->bTongueSpawned = false;
			Froggy->Consume(Target);

			Destroy();
		}
	}
}


// Called when the game starts or when spawned
void ATongueProjectile::BeginPlay()
{
	Super::BeginPlay();

	Froggy = Cast<AFrogGameCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (Froggy)
	{
		// Just taking the X scale since the scale should be uniform
		const float ActualRange{TongueRange * Froggy->GetActorScale().X};
		TargetLocation = Froggy->GetRayMesh()->GetComponentLocation() + (Froggy->GetActorForwardVector() * ActualRange);
		if(Froggy->CurrentTarget)
		{
			Target = Froggy->CurrentTarget;
		}
	}
}

// Called every frame
void ATongueProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bShouldReturn)
	{
		SeekTarget(DeltaTime);
	}
	else
	{
		Return(DeltaTime);
	}
}
