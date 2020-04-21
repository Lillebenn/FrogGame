// Fill out your copyright notice in the Description page of Project Settings.


#include "SphereDrop.h"
#include "Engine/StaticMesh.h"
#include "FrogGameCharacter.h"
#include "GameFramework/PlayerController.h"
#include "EdibleComponent.h"
#include "Materials/Material.h"

// Sets default values
ASphereDrop::ASphereDrop()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetCollisionProfileName(TEXT("SphereDrop"));

	EdibleComponent = CreateDefaultSubobject<UEdibleComponent>(TEXT("Edible Info"));
	RootComponent = StaticMesh;
}

// Called when the game starts or when spawned
void ASphereDrop::BeginPlay()
{
	Super::BeginPlay();

	Frog = Cast<AFrogGameCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter());
	SwirlInfo.LinearUpPosition = InitialZPosition;
	SwirlInfo.Construct();
	GetWorld()->GetTimerManager().SetTimer(SphereLifeSpan, this, &ASphereDrop::Consume, 2.5f);
	if(Frog)
	{
		if(Frog->CurrentMode == ECharacterMode::Neutral)
		{
			InitialRadius = InitialRadius * 1.5f;
		}else
		{
			SwirlInfo.LinearUpSpeed = SwirlInfo.LinearUpSpeed * 2.f;
			SwirlInfo.LinearInSpeed = 0;
		}
	}
}

void ASphereDrop::MoveToPlayer(const float DeltaTime)
{
	FVector Direction = GetActorLocation() - Frog->GetActorLocation();
	Direction.Normalize();
	StartPosition = Frog->GetActorLocation() + Direction * InitialRadius;
	StartPosition.Z = Frog->GetActorLocation().Z + InitialZPosition;

	const FVector NewPosition{
		FMath::VInterpConstantTo(GetActorLocation(), StartPosition, DeltaTime, MoveToPlayerSpeed)
	};
	SetActorLocation(NewPosition);
}

void ASphereDrop::Consume()
{
	if (Frog)
	{
		Frog->Consume(this);
	}
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
			const float RadialDistance{
				FFrogLibrary::SquaredRadialDistance(GetActorLocation(), Frog->GetActorLocation())
			};
			if (RadialDistance <= InitialRadius * InitialRadius)
			{
				SwirlInfo.CurrentRadius = FMath::Sqrt(RadialDistance);
				const FVector2D FrogXY{GetActorLocation() - Frog->GetActorLocation()};
				SwirlInfo.RadianDelta = FMath::Atan2(FrogXY.Y, FrogXY.X);

				bShouldSwirl = true;
			}
		}
		else
		{
			FVector NewPosition;
			if (FFrogLibrary::Swirl(DeltaTime, SwirlInfo, Frog->GetActorLocation(), NewPosition))
			{
				Consume();
			}
			else
			{
				SetActorLocation(NewPosition);
			}
		}
	}

}
