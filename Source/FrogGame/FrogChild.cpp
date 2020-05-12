// Fill out your copyright notice in the Description page of Project Settings.


#include "FrogChild.h"


#include "Animation/AnimInstance.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"

AFrogChild::AFrogChild()
{
	GetCapsuleComponent()->SetGenerateOverlapEvents(true);
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("FrogChild"));

	DirectionArrow = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Arrow Marker"));
	DirectionArrow->SetupAttachment(RootComponent);
	DirectionArrow->SetCollisionProfileName(TEXT("NoCollision"));
	DirectionArrow->SetRelativeLocation(FVector(0.f, 0.f, 320.f));
	DirectionArrow->SetRelativeScale3D(FVector(0.83f));
	DirectionArrow->SetCastShadow(false);
	EdibleComponent->bAllowSuction = false;
	EdibleComponent->ScorePoints = 500000;

	GetMesh()->SetRelativeScale3D(FVector(4.f));
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -85.f));
}

void AFrogChild::MoveToSwamp()
{
	if(FrogScreech)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), FrogScreech, GetActorLocation(), FRotator());
	}
	bIsCollected = true;
	SetActorTransform(SwampLocation);
	DirectionArrow->SetVisibility(false);
	PatrolSettings.PathPoints.Empty();
}
