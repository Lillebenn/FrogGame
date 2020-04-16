// Fill out your copyright notice in the Description page of Project Settings.


#include "ArtBibleDisplayer.h"
#include "Engine.h"
#include "FileLoader.h"


// Sets default values
AArtBibleDisplayer::AArtBibleDisplayer()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DisplayedObject = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Displayed Object"));
	RootComponent = DisplayedObject;
	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetRelativeRotation(FRotator(-15.f, 0.f, 0.f));
}

// Called when the game starts or when spawned
void AArtBibleDisplayer::BeginPlay()
{
	Super::BeginPlay();
	MeshArrayFromList();
	if (StaticMeshes.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to find any meshes to display!"));
		return;
	}
	CurrentDelay = DelayTime;
	Index = NextIndex;
	// Get the first object to display.
	SwitchDisplayedObject();

	NextIndex++;
}

// Called every frame
void AArtBibleDisplayer::Tick(float DeltaTime)
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
	if (bRotate)
	{
		FRotator NewRotation{CameraBoom->GetRelativeRotation()};
		CurYaw += RotationSpeed * DeltaTime;
		NewRotation.Yaw = CurYaw;

		CameraBoom->SetRelativeRotation(NewRotation);
	}
}


void AArtBibleDisplayer::SwitchDisplayedObject()
{
	if (Index >= StaticMeshes.Num())
	{
		Index = bLoop ? 0 : Index - 1;
		NextIndex = Index + 1;

		//GEngine->AddOnScreenDebugMessage(0, 5.f, FColor::Red, TEXT("Reached the end of the list!"));
	}
	DisplayedObject->SetStaticMesh(StaticMeshes[Index]);
	const FSphere Bounds{DisplayedObject->Bounds.GetSphere()};
	CameraBoom->TargetArmLength = Bounds.W * 10.f;
	if (bCameraZOffset)
	{
		FVector NewOffset{CameraBoom->GetRelativeLocation()};
		NewOffset.Z = Bounds.W;
		CameraBoom->TargetOffset = NewOffset;
	}
	CurYaw = 0.f;
}

void AArtBibleDisplayer::MeshArrayFromList()
{
	for (auto Directory : AutoFillFolders)
	{
		Directory += TEXT("/");
		FString ActualPath{FPaths::Combine(FPaths::ProjectUserDir(), TEXT("Content/"), Directory)};
		if (FPaths::DirectoryExists(ActualPath))
		{
			TArray<FString> Objects{FileLoader::GetAllFilesInDirectory(ActualPath)};
			for (auto Object : Objects)
			{
				UStaticMesh* Mesh{FileLoader::LoadMeshFromPath(*Object)};
				if (Mesh)
				{
					StaticMeshes.Add(Mesh);
				}
			}
		}
	}
}
