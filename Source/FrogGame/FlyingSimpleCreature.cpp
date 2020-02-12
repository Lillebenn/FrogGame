// Fill out your copyright notice in the Description page of Project Settings.


#include "FlyingSimpleCreature.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"


AFlyingSimpleCreature::AFlyingSimpleCreature()
{
	NavCollider = CreateDefaultSubobject<UCapsuleComponent>(TEXT("NavCollider"));
	NavCollider->InitCapsuleSize(15.f, 80.0f);
	NavCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = NavCollider;
}

void AFlyingSimpleCreature::BeginPlay()
{
	Super::BeginPlay();
	GetMesh()->SetRelativeLocation(FVector(0, 0, 50.f));
}
