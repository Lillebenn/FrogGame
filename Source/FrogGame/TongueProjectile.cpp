// Fill out your copyright notice in the Description page of Project Settings.


#include "TongueProjectile.h"
#include "FrogGameCharacter.h"
#include "CableComponent.h"
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
	CollisionSphere->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);
	CollisionSphere->SetNotifyRigidBodyCollision(true);
	CollisionSphere->OnComponentHit.AddDynamic(this, &ATongueProjectile::OnComponentHit);
	// Set the SphereComponent as the root component.
	RootComponent = CollisionSphere;


	//Create the static mesh component 
	TongueMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TongueMesh"));
	TongueMesh->SetSimulatePhysics(false);
	TongueMesh->SetCollisionProfileName("NoCollision");
	TongueMesh->SetupAttachment(RootComponent);
	CurrentPause = PauseDuration;
}

void ATongueProjectile::VInterpTo(const FVector InterpTo, const float TongueSpeed, const float DeltaTime)
{
	FHitResult HitResult = FHitResult(ForceInit);
	const bool bSweep = !bShouldReturn; // If the tongue is returning, we don't care about checking for collisions

	SetActorLocation(FMath::VInterpConstantTo(GetActorLocation(),
	                                          InterpTo,
	                                          DeltaTime, TongueSpeed), bSweep, &HitResult);
	if (Target)
	{
		// Get it to return in case it doesn't find a collision
		if (FVector::Dist(GetActorLocation(), InterpTo) <= 0.f)
		{
			AttachEdible(Target);
		}

		return;
	}

	if (HitResult.IsValidBlockingHit())
	{
		Target = HitResult.GetActor();
		bShouldReturn = true;
	}
}

void ATongueProjectile::AttachEdible(AActor* EdibleActor)
{
	if (EdibleActor->Implements<UEdible>())
	{
		const FAttachmentTransformRules InRule(EAttachmentRule::KeepWorld, false);
		EdibleActor->AttachToActor(this, InRule);

		// Turn off collision on the dragged object so we don't get affected by it on the way back.
		EdibleActor->SetActorEnableCollision(false);
		GetWorld()->GetFirstPlayerController()->PlayerCameraManager->PlayCameraShake(TongueShakeEffect);

		IEdible::Execute_OnDisabled(EdibleActor);
		bShouldReturn = true;
	}
}


void ATongueProjectile::OnComponentHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                       FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor->Implements<UEdible>())
	{
		AttachEdible(OtherActor);
	}
}

void ATongueProjectile::SeekTarget(const float DeltaTime)
{
	if (Target)
	{
		USceneComponent* TargetComponent{IEdible::Execute_GetTargetComponent(Target)};
		if(TargetComponent)
		{
			VInterpTo(IEdible::Execute_GetTargetComponent(Target)->GetComponentLocation(), TongueSeekSpeed, DeltaTime);
		}else
		{
			VInterpTo(Target->GetActorLocation(), TongueSeekSpeed, DeltaTime);
			UE_LOG(LogTemp, Error, TEXT("Missing Target Component Setup in %s!"), *Target->GetName());
		}
	}
	else
	{
		VInterpTo(TargetLocation, TongueSeekSpeed, DeltaTime);
		// Keep going until you hit something or reach the max distance
		if (FVector::Dist(TargetLocation, GetActorLocation()) <= 0.5f)
		{
			bShouldReturn = true;
		}
	}
}

void ATongueProjectile::Return(const float DeltaTime)
{
	if (CurrentPause <= 0.f)
	{
		bIsPaused = false;
	}
	else
	{
		CurrentPause -= DeltaTime;
	}
	if (!bIsPaused)
	{
		const FVector ReturnPos{Froggy->GetTongueStart()->GetComponentLocation()};
		VInterpTo(ReturnPos, TongueReturnSpeed, DeltaTime);
		if (FVector::Dist(GetActorLocation(), ReturnPos) <= CollisionSphere->GetScaledSphereRadius())
		{
			Froggy->Consume(Target, this);
		}
		CurrentPause = PauseDuration;
	}
}


// Called when the game starts or when spawned
void ATongueProjectile::BeginPlay()
{
	Super::BeginPlay();

	Froggy = Cast<AFrogGameCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (!Froggy)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to find FrogCharacter reference!"));
		Destroy();
	}
}

void ATongueProjectile::ActivateTongue(AActor* InTarget)
{
	Cable = NewObject<UCableComponent>(Froggy, UCableComponent::StaticClass());

	Cable->CableLength = 0.f;
	Cable->NumSegments = 10;
	Cable->CableGravityScale = 0.f;
	Cable->SolverIterations = 3;
	Cable->bEnableStiffness = false;
	Cable->CableWidth = Froggy->CurrentCableWidth;
	Cable->EndLocation = FVector(0, 0, 0); // Zero vector seems to bug

	USceneComponent* AttachComponent{Froggy->TongueStart};
	const FVector Location{AttachComponent->GetComponentTransform().GetLocation()};
	const FRotator Rotation{AttachComponent->GetComponentTransform().GetRotation()};
	Cable->SetRelativeLocation(Location);
	Cable->SetRelativeRotation(Rotation);
	const FAttachmentTransformRules InRule(EAttachmentRule::KeepWorld, false);
	Cable->AttachToComponent(AttachComponent, InRule);

	Cable->SetMaterial(0, CableMaterial);

	Cable->SetAttachEndTo(this, FName());
	Cable->RegisterComponent();
	if (!InTarget)
	{
		// Just taking the X scale since the scale should be uniform
		const float ActualRange{TongueRange * Froggy->GetActorScale().X};
		TargetLocation = Froggy->GetTongueStart()->GetComponentLocation() + (Froggy->GetActorForwardVector() *
			ActualRange);
	}
	else
	{
		Target = InTarget;
	}
	TongueSeekSpeed = Froggy->TongueSeekSpeed;
	TongueReturnSpeed = Froggy->TongueReturnSpeed;
	Activated = true;
}


// Called every frame
void ATongueProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (Activated)
	{
		if (!bShouldReturn)
		{
			SeekTarget(DeltaTime);
		}
		else
		{
			Return(DeltaTime);
		}
	}
}
