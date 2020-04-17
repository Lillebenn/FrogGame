// Fill out your copyright notice in the Description page of Project Settings.


#include "ArtBibleDisplayer.h"
#include "Engine.h"
#include "FileLoader.h"


// Sets default values
AArtBibleDisplayer::AArtBibleDisplayer()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SkelMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Skeletal Mesh"));
	RootComponent = SkelMesh;
	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetRelativeRotation(FRotator(-15.f, 0.f, 0.f));
	DisplayedObject = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Displayed Object"));
	DisplayedObject->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AArtBibleDisplayer::BeginPlay()
{
	Super::BeginPlay();
	MeshArrayFromList();
	NumMeshes = StaticMeshes.Num() + SkeletalMeshes.Num();
	if (NumMeshes == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to find any meshes to display!"));
		return;
	}
	if (SkeletalMeshes.Num() == 0)
	{
		RootComponent = DisplayedObject;
		CameraBoom->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
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
	if (NumMeshes == 0)
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
	if (Index >= NumMeshes)
	{
		Index = bLoop ? 0 : Index - 1;
		NextIndex = Index + 1;

		//GEngine->AddOnScreenDebugMessage(0, 5.f, FColor::Red, TEXT("Reached the end of the list!"));
	}
	float BoundsRadius;
	if (Index < NumSkeletalMeshes)
	{
		if (RootComponent != SkelMesh)
		{
			SetDisplayType(true);
		}
		SkelMesh->SetSkeletalMesh(SkeletalMeshes[Index]);
		BoundsRadius = SkelMesh->Bounds.GetSphere().W;
	}
	else
	{
		if (RootComponent != DisplayedObject)
		{
			SetDisplayType(false);
		}
		DisplayedObject->SetStaticMesh(StaticMeshes[Index]);
		BoundsRadius = DisplayedObject->Bounds.GetSphere().W;
	}
	CameraBoom->TargetArmLength = BoundsRadius * 10.f;
	if (bCameraZOffset)
	{
		FVector NewOffset{CameraBoom->GetRelativeLocation()};
		NewOffset.Z = BoundsRadius;
		CameraBoom->TargetOffset = NewOffset;
	}
	CurYaw = 0.f;
}

void AArtBibleDisplayer::SetDisplayType(const bool bToSkeletal)
{
	if (bToSkeletal)
	{
		RootComponent = SkelMesh;
		DisplayedObject->SetStaticMesh(nullptr);
	}
	else
	{
		RootComponent = DisplayedObject;
		SkelMesh->SetSkeletalMesh(nullptr);
	}
	CameraBoom->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
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
			if (!bOnlyStatic)
			{
				for (auto Object : Objects)
				{
					USkeletalMesh* Mesh{FileLoader::LoadSkeletalMeshFromPath(*Object)};
					if (Mesh)
					{
						SkeletalMeshes.Add(Mesh);
					}
				}
			}
			if (!bOnlySkeletal)
			{
				for (auto Object : Objects)
				{
					UStaticMesh* Mesh{FileLoader::LoadStaticMeshFromPath(*Object)};
					if (Mesh)
					{
						StaticMeshes.Add(Mesh);
					}
				}
			}
		}
	}
}
