// Fill out your copyright notice in the Description page of Project Settings.


#include "ArtBibleDisplay.h"
#include "CineCameraComponent.h"
#include "Engine.h"
#include "FFrogLibrary.h"


// Sets default values
AArtBibleDisplay::AArtBibleDisplay()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DisplayedObject = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Displayed Object"));
	RootComponent = DisplayedObject;
	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetRelativeRotation(FRotator(-15.f, 0.f, 0.f));
	CineCam = CreateDefaultSubobject<UCineCameraComponent>(TEXT("Camera Component"));
	CineCam->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
}

// Called when the game starts or when spawned
void AArtBibleDisplay::BeginPlay()
{
	Super::BeginPlay();
	MeshArrayFromList();
	if (StaticMeshes.Num() == 0)
	{
		return;
	}
	CurrentDelay = DelayTime;
	Index = NextIndex;
	// Get the first object to display.
	SwitchDisplayedObject();

	NextIndex++;
}

// Called every frame
void AArtBibleDisplay::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (StaticMeshes.Num() == 0)
	{
		return;
	}
	if (bAutoDisplay)
	{
		if (bFullRotation)
		{
			if (CurYaw >= 360.f)
			{
				Index = NextIndex;
				NextIndex++;
				SwitchDisplayedObject();
			}
		}
		else
		{
			CurrentDelay -= DeltaTime;
			if (CurrentDelay <= 0.f)
			{
				Index = NextIndex;
				NextIndex++;
				SwitchDisplayedObject();
				CurrentDelay = DelayTime;
			}
		}
	}
	else if (NextIndex != Index + 1)
	{
		// Reset Current Delay so we start back at max timer if bAutoDisplay is ticked again.
		CurrentDelay = DelayTime;
		Index = NextIndex;
		SwitchDisplayedObject();
	}
	if (bShouldRotate)
	{
		FRotator NewRotation{CameraBoom->GetRelativeRotation()};
		NewRotation.Yaw += RotationSpeed * DeltaTime;
		CurYaw += RotationSpeed * DeltaTime;
		CameraBoom->SetRelativeRotation(NewRotation);
	}
}


void AArtBibleDisplay::SwitchDisplayedObject()
{
	if (Index >= StaticMeshes.Num())
	{
		Index = bShouldLoop ? 0 : Index - 1;
		NextIndex = Index + 1;

		GEngine->AddOnScreenDebugMessage(0, 5.f, FColor::Red, TEXT("Reached the end of the list!"));
	}
	DisplayedObject->SetStaticMesh(StaticMeshes[Index]);
	FBox Bounds{DisplayedObject->Bounds.GetBox()};
	const float GreatestDistance{FVector::Dist(Bounds.GetCenter(), Bounds.Max)};
	const float NewDistance{GreatestDistance * DistanceMultiplier};
	CameraBoom->TargetArmLength = NewDistance;
	FVector NewOffset{CameraBoom->GetRelativeLocation()};
	NewOffset.Z = NewDistance / 10.f;
	CameraBoom->TargetOffset = NewOffset;
	CurYaw = 0.f;
}

void AArtBibleDisplay::MeshArrayFromList()
{
	for (auto Directory : AutoFillFolders)
	{
		Directory += TEXT("/");
		FString ActualPath{FPaths::Combine(FPaths::ProjectUserDir(), TEXT("Content/"), Directory)};
		if (FPaths::DirectoryExists(ActualPath))
		{
			TArray<FString> Objects{FFrogLibrary::GetAllFilesInDirectory(ActualPath)};
			for (auto Object : Objects)
			{
				UStaticMesh* Mesh{FFrogLibrary::LoadMeshFromPath(*Object)};
				if (Mesh)
				{
					StaticMeshes.Add(Mesh);
				}
			}
		}
	}
}
