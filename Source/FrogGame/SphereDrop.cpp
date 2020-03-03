// Fill out your copyright notice in the Description page of Project Settings.


#include "SphereDrop.h"
#include "Engine/StaticMesh.h"
#include "FrogGameCharacter.h"
#include "GameFramework/PlayerController.h"

// Sets default values
ASphereDrop::ASphereDrop()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetCollisionProfileName(TEXT("SphereDrop"));

	RootComponent = StaticMesh;
}

// Called when the game starts or when spawned
void ASphereDrop::BeginPlay()
{
	Super::BeginPlay();

	Frog = Cast<AFrogGameCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter());
	LinearUpPosition = InitialZPosition;
}

void ASphereDrop::MoveToPlayer(const float DeltaTime)
{
	FVector Direction = GetActorLocation() - Frog->GetActorLocation();
	Direction.Normalize();
	StartPosition = Frog->GetActorLocation() + Direction * InitialRadius;
	StartPosition.Z = Frog->GetActorLocation().Z + InitialZPosition;

	const FVector NewPosition{FMath::VInterpConstantTo(GetActorLocation(), StartPosition, DeltaTime, 500.f)};
	SetActorLocation(NewPosition);
}

void ASphereDrop::Swirl(const float DeltaTime)
{
	if (CurrentRadius <= MinRadius)
	{
		Destroy();
	}
	if (RadianDelta >= 2.f * PI)
	{
		RadianDelta = 0.f;
	}

	RadianDelta += AngularSpeed * DeltaTime;
	CurrentRadius -= LinearInSpeed * DeltaTime;
	if (LinearUpPosition <= MaxZPosition)
	{
		LinearUpPosition += LinearUpSpeed * DeltaTime;
	}
	const FVector FrogPos{Frog->GetActorLocation()};
	const float X{FrogPos.X + CurrentRadius * FMath::Cos(RadianDelta)};
	const float Y{FrogPos.Y + CurrentRadius * FMath::Sin(RadianDelta)};
	const float Z{FrogPos.Z + LinearUpPosition};
	SetActorLocation(FVector{X, Y, Z});
}

void ASphereDrop::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (Frog)
	{
		Frog->IncreaseScale(EdibleInfo);
	}
	Super::EndPlay(EndPlayReason);
}

// Called every frame
void ASphereDrop::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (Frog)
	{
		if (!bShouldSwirl)
		{
			MoveToPlayer(DeltaTime);
			const float RadialDistance{FindRadialDistance()};
			if (RadialDistance <= InitialRadius * InitialRadius)
			{
				CurrentRadius = FMath::Sqrt(RadialDistance);
				const FVector2D FrogXY{GetActorLocation() - Frog->GetActorLocation()};
				RadianDelta = FMath::Atan2(FrogXY.Y, FrogXY.X);

				bShouldSwirl = true;
			}
		}
		else
		{
			Swirl(DeltaTime);
		}
	}
}

float ASphereDrop::FindRadialDistance() const
{
	const FVector2D FrogXY{GetActorLocation() - Frog->GetActorLocation()};
	const float SquaredRadialDistance{(FrogXY.X * FrogXY.X) + (FrogXY.Y * FrogXY.Y)};
	return SquaredRadialDistance;
}
