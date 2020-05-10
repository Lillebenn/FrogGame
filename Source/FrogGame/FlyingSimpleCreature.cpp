// Fill out your copyright notice in the Description page of Project Settings.


#include "FlyingSimpleCreature.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "CustomDestructibleComponent.h"
#include "FrogGameCharacter.h"


AFlyingSimpleCreature::AFlyingSimpleCreature()
{
	Nav = CreateDefaultSubobject<USphereComponent>(TEXT("NavCollider"));
	Nav->InitSphereRadius(5.f);
	Nav->SetCollisionProfileName(TEXT("NoCollision"));
	Nav->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Nav->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	Nav->SetupAttachment(RootComponent);
	Nav->bEditableWhenInherited = true;
}

void AFlyingSimpleCreature::BeginPlay()
{
	Super::BeginPlay();
	Nav->SetRelativeLocation(FVector(
		0.f, 0.f, -(Nav->GetScaledSphereRadius() + CapsuleComponent->GetScaledCapsuleHalfHeight())));
}


