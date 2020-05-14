// Fill out your copyright notice in the Description page of Project Settings.


#include "SpatialPartition.h"
#include "DestructibleObject.h"
#include "Edible.h"
#include "TimerManager.h"
#include "Async/Async.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"

// Sets default values
ASpatialPartition::ASpatialPartition()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	DisableVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("DisableVolume"));
	DisableVolume->SetCollisionProfileName(TEXT("OverlapEdible"));
	DisableVolume->SetBoxExtent(FVector(3072.f));
	DisableVolume->SetCanEverAffectNavigation(false);
	DisableVolume->ShapeColor = FColor::Red;
	RootComponent = DisableVolume;
	EnableVolume = CreateDefaultSubobject<USphereComponent>(TEXT("EnableVolume"));
	EnableVolume->SetupAttachment(RootComponent);
	EnableVolume->SetCollisionProfileName(TEXT("PawnOnly"));
	EnableVolume->SetSphereRadius(12288.f);
	EnableVolume->SetCanEverAffectNavigation(false);
	EnableVolume->ShapeColor = FColor::Emerald;
}


void ASpatialPartition::InitDisableActors()
{
	TArray<AActor*> OverlappingActors;
	DisableVolume->GetOverlappingActors(OverlappingActors);
	for (auto Actor : OverlappingActors)
	{
		if (Actor->Implements<UEdible>())
		{
			if (bDisableAtStart)
			{
				IEdible::Execute_SetActive(Actor, false);
			}else
			{
				IEdible::Execute_SetActive(Actor, true);
			}

			DisabledActors.Add(Actor);
		}
	}
}

// Called when the game starts or when spawned
void ASpatialPartition::BeginPlay()
{
	Super::BeginPlay();
	GetWorld()->GetTimerManager().SetTimer(Test, this, &ASpatialPartition::InitDisableActors, 2.7f);

	EnableVolume->OnComponentBeginOverlap.AddDynamic(this, &ASpatialPartition::OnPlayerOverlap);
	EnableVolume->OnComponentEndOverlap.AddDynamic(this, &ASpatialPartition::OnPlayerEndOverlap);
}

void ASpatialPartition::OnPlayerOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                        const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Detected player"))
	for (auto Actor : DisabledActors)
	{
		if(Actor)
		{
			if (!IEdible::Execute_IsActive(Actor))
			{
				IEdible::Execute_SetActive(Actor, true);
			}
		}
	}
}

void ASpatialPartition::OnPlayerEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                           UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	for (auto Actor : DisabledActors)
	{
		if(Actor)
		{
			IEdible::Execute_SetActive(Actor, false);
		}
	}
}
