// Fill out your copyright notice in the Description page of Project Settings.


#include "StaticAdvCreature.h"

#include "AIController.h"
#include "BrainComponent.h"


AStaticAdvCreature::AStaticAdvCreature()
{
	CreatureMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Creature Mesh"));
	CreatureMesh->SetReceivesDecals(false);
	CreatureMesh->SetCollisionProfileName(TEXT("NoCollision"));
	CreatureMesh->SetupAttachment(RootComponent);
}

UStaticMeshComponent* AStaticAdvCreature::GetStaticMesh()
{
	return CreatureMesh;
}
bool AStaticAdvCreature::IsDisabled_Implementation()
{
	return bShouldDestroy;
}
void AStaticAdvCreature::DisableActor_Implementation()
{
	// Not 100% sure if this is necessary, but we don't need the AI to keep running after being snatched.
	AController* AI{GetController()};
	if (AI)
	{
		AI->StopMovement();
		AI->Destroy();
	}

	bShouldDestroy = true;
}

void AStaticAdvCreature::PauseAI_Implementation(bool bPause)
{
	AAIController* AI{Cast<AAIController>(GetController())};
	if (AI)
	{
		if (bPause)
		{
			AI->BrainComponent->PauseLogic(TEXT("Hit by whirlwind."));
			AI->StopMovement();
		}
		else
		{
			AI->BrainComponent->ResumeLogic(TEXT("No longer hit by whirlwind."));
		}
	}
}
