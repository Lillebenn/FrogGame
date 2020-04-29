// Fill out your copyright notice in the Description page of Project Settings.


#include "EdibleComponent.h"
#include "Engine/World.h"
#include "FrogGameCharacter.h"
#include "Components/SkeletalMeshComponent.h"

// Sets default values for this component's properties
UEdibleComponent::UEdibleComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

FTransform UEdibleComponent::GetInitialTransform() const
{
	return InitialTransform;
}

void UEdibleComponent::SetInitialTransform()
{
	InitialTransform = GetOwner()->GetActorTransform();
}


void UEdibleComponent::IgnorePawnCollision() const
{
	UStaticMeshComponent* StaticMesh{GetOwner()->FindComponentByClass<UStaticMeshComponent>()};
	if (StaticMesh)
	{
		StaticMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		StaticMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	}
	else if (USkeletalMeshComponent* SkeletalMesh = GetOwner()->FindComponentByClass<USkeletalMeshComponent>())
	{
		SkeletalMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		SkeletalMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	}
}

// Called when the game starts
void UEdibleComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
}


// Called every frame
void UEdibleComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                     FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}
