// Fill out your copyright notice in the Description page of Project Settings.


#include "AdvCreature.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Controller.h"
#include "FrogGameInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "EdibleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "SphereDrop.h"

// Sets default values
AAdvCreature::AAdvCreature()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	EdibleComponent = CreateDefaultSubobject<UEdibleComponent>(TEXT("Edible Info"));

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

// Called when the game starts or when spawned
void AAdvCreature::BeginPlay()
{
	Super::BeginPlay();


	StartTransform = GetTransform();
	GetCapsuleComponent()->SetCollisionObjectType(ECC_GameTraceChannel1);
	// For capsules we just use the radius value. Could potentially do a combination/average of the half-height and radius if the creature is particularly tall.
	CalculateBoundingSize();
}

// Called every frame
void AAdvCreature::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AAdvCreature::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

bool AAdvCreature::IsDisabled_Implementation()
{
	return ShouldDestroy;
}


UEdibleComponent* AAdvCreature::GetInfo_Implementation() const
{
	return EdibleComponent;
}

void AAdvCreature::DisableActor_Implementation()
{
	AController* AI{GetController()};
	if (AI)
	{
		AI->StopMovement();
		AI->UnPossess();
		AI->Destroy();
	}
}

void AAdvCreature::OnDisabled_Implementation()
{
	DisableActor_Implementation();
	UFrogGameInstance* FrogInstance{Cast<UFrogGameInstance>(GetWorld()->GetGameInstance())};
	if (FrogInstance)
	{
		FrogInstance->OnActorDestroyed(this);
	}
}


// Custom behaviour when saving or loading
void AAdvCreature::ActorSaveDataLoaded_Implementation()
{
}

FTransform AAdvCreature::GetStartTransform()
{
	return StartTransform;
}

void AAdvCreature::ActorSaveDataSaved_Implementation()
{
}

void AAdvCreature::CalculateBoundingSize()
{
	if (GetMesh())
	{
		const FVector RoughSize = GetMesh()->Bounds.GetBox().GetSize();
		const FVector AbsoluteSize{RoughSize.GetAbsMin()};
		// Get the average axis value of the bounding box
		EdibleComponent->Size = (AbsoluteSize.X + AbsoluteSize.Y + AbsoluteSize.Z) / 6;
		if (EdibleComponent->bAutomaticSizeTier)
		{
			IEdible::CalculateSizeTier(EdibleComponent);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("%s is missing a mesh!"), *GetName());
	}
}

void AAdvCreature::SpawnSpheres() const
{
	if (EdibleComponent->Drop)
	{
		const float SphereSize{EdibleComponent->GetSphereSize()};
		for (int i{0}; i < 5; i++)
		{
			const FVector2D SpawnLocation2D{FMath::RandPointInCircle(125.f)};
			const FVector SpawnLocation{
				GetActorLocation().X + SpawnLocation2D.X, GetActorLocation().Y + SpawnLocation2D.Y, GetActorLocation().Z
			};
			const FTransform SpawnTransform{SpawnLocation};
			ASphereDrop* Sphere{GetWorld()->SpawnActorDeferred<ASphereDrop>(EdibleComponent->Drop, SpawnTransform)};
			Sphere->EdibleComponent = EdibleComponent;
			Sphere->EdibleComponent->Size = SphereSize;
			UGameplayStatics::FinishSpawningActor(Sphere, SpawnTransform);
		}
	}
}
