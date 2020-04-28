// Fill out your copyright notice in the Description page of Project Settings.


#include "FlyingAdvCreature.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"

AFlyingAdvCreature::AFlyingAdvCreature()
{
	NavCollider = CreateDefaultSubobject<USphereComponent>(TEXT("NavCollider"));
	NavCollider->InitSphereRadius(5.f);
	NavCollider->SetCollisionProfileName(TEXT("NoCollision"));
	NavCollider->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	NavCollider->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	NavCollider->SetupAttachment(RootComponent);
	NavCollider->SetRelativeLocation(FVector(
		0.f, 0.f, -(NavCollider->GetScaledSphereRadius() + GetCapsuleComponent()->GetScaledCapsuleHalfHeight())));
}

void AFlyingAdvCreature::BeginPlay()
{
	Super::BeginPlay();
	NavCollider->SetRelativeLocation(FVector(
		0.f, 0.f, -(NavCollider->GetScaledSphereRadius() + GetCapsuleComponent()->GetScaledCapsuleHalfHeight())));
}
