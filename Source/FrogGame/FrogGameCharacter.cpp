// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "FrogGameCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "Components/InputComponent.h"
#include "Engine.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "TongueProjectile.h"
#include "DrawDebugHelpers.h"
#include "CableComponent.h"
#include "Edible.h"

//////////////////////////////////////////////////////////////////////////
// AFrogGameCharacter

AFrogGameCharacter::AFrogGameCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);


	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	// Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	BoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxTrace"));
	BoxCollider->SetupAttachment(RootComponent);
	BoxCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BoxCollider->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);
	// Create a spawn point for linetrace, only used to linetrace so does not need to ever be visible.
	RayMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RayMesh"));
	RayMesh->SetupAttachment(RootComponent);
	RayMesh->SetVisibility(false);

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

void AFrogGameCharacter::BeginPlay()
{
	Super::BeginPlay();
	const FVector Viewport{GetWorld()->GetGameViewport()->Viewport->GetSizeXY()};
	BoxCollider->SetBoxExtent(FVector(Tongue.GetDefaultObject()->TongueRange / 2.f, Viewport.X / 2.f,
	                                  Viewport.Y / 2.f));
	BoxCollider->SetRelativeLocation(FVector(BoxCollider->GetUnscaledBoxExtent().X, 0, 0));
}

UStaticMeshComponent* AFrogGameCharacter::GetRayMesh()
{
	return RayMesh;
}

//////////////////////////////////////////////////////////////////////////
// Input

void AFrogGameCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Eat", IE_Pressed, this, &AFrogGameCharacter::Lickitung);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AFrogGameCharacter::JumpCharge);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AFrogGameCharacter::FroggyJump);

	PlayerInputComponent->BindAxis("MoveForward", this, &AFrogGameCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AFrogGameCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AFrogGameCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AFrogGameCharacter::LookUpAtRate);
}


void AFrogGameCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UCharacterMovementComponent* CharMovement = GetCharacterMovement();
	if (CharMovement)
	{
		CharMovement->JumpZVelocity = BaseJump;
	}
	if(IsHolding)
		{
		   Jumptick(DeltaTime);
		}
	if (Lerping)
	{
		if (ScaleLerp < 1.0f)
		{
			ScaleLerp += DeltaTime;
			const FVector CurrentScale{GetActorScale()};
			SetActorScale3D(FMath::Lerp(GetActorScale(), DesiredScale, ScaleLerp));
			const float ScaleDelta{(GetActorScale() - CurrentScale).X};
			UpdateCameraBoom(ScaleDelta);
		}
	}
	AutoAim();
}

void AFrogGameCharacter::AutoAim()
{
	TArray<AActor*> OverlappingActors;
	BoxCollider->GetOverlappingActors(OverlappingActors);
	for (AActor* Actor : OverlappingActors)
	{
		if (Actor->Implements<UEdible>())
		{
			const FEdibleInfo SizeInfo{IEdible::Execute_GetInfo(Actor)};
			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red,
			                                 FString::Printf(TEXT("%s is in view."), *Actor->GetName()));
			// If the actor's size is less than or equal to the frog's size 
			if (SizeInfo.SizeTier < SizeTier && SizeInfo.SizeTier >= SizeTier - 2)
			{
			}
		}
	}
}

void AFrogGameCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AFrogGameCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AFrogGameCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AFrogGameCharacter::Consume(AActor* OtherActor)
{
	if (Cable)
	{
		Cable->DestroyComponent();
		bTongueSpawned = false;
	}
	if (!OtherActor)
	{
		return;
	}
	if (OtherActor->Implements<UEdible>())
	{
		const FEdibleInfo SizeInfo{IEdible::Execute_GetInfo(OtherActor)};
		// This probably happens much earlier than here. (When the target is being calculated)
		OtherActor->Destroy();
		// just reset the lerp values
		ScaleLerp = 0.0f;
		Lerping = true;
		// We use the scaled radius value of the capsule collider to get an approximate size value for the main character.
		const float ScaledRadius{GetCapsuleComponent()->GetScaledCapsuleRadius()};
		// Compare this to the averaged bounding box size of the object being eaten and factor in the growth coefficient.
		const float SizeDiff{SizeInfo.Size / ScaledRadius * SizeInfo.GrowthCoefficient};
		// If SizeInfo.Size = 10 and ScaledRadius = 50 then we get a value of 10/50 = 0.2 or 20%.
		// Increase actor scale by this value. 
		DesiredScale = GetActorScale() * (1 + SizeDiff);
	}
}


void AFrogGameCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AFrogGameCharacter::MoveRight(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void AFrogGameCharacter::UpdateCameraBoom(const float ScaleDelta)
{
	const float NewDistance = CameraBoom->TargetArmLength + ((CameraBoom->TargetArmLength / 100) * ScaleDelta);
	CameraBoom->TargetArmLength = NewDistance;
	FVector Extent{BoxCollider->GetUnscaledBoxExtent()};
	Extent.X = (Tongue.GetDefaultObject()->TongueRange / 2.f) * GetActorScale().X;
	const float XDiff{Extent.X - BoxCollider->GetUnscaledBoxExtent().X};
	BoxCollider->SetBoxExtent(Extent);
	FVector NewPosition{BoxCollider->GetRelativeLocation()};
	NewPosition.X += XDiff;
	BoxCollider->SetRelativeLocation(NewPosition);
}

void AFrogGameCharacter::Lickitung()
{
	// TODO: When activating this, set the target's collision channel to a custom one that only that object has. 
	if (!bTongueSpawned)
	{
		Cable = NewObject<UCableComponent>(this, UCableComponent::StaticClass());

		Cable->CableLength = 0.f;
		Cable->NumSegments = 1;
		Cable->CableGravityScale = 0.f;
		Cable->SolverIterations = 3;
		Cable->EndLocation = FVector(5, 0, 0); // Zero vector seems to bug


		const FVector Location{RayMesh->GetComponentTransform().GetLocation()};
		const FRotator Rotation{RayMesh->GetComponentTransform().GetRotation()};
		Cable->SetRelativeLocation(Location);
		Cable->SetRelativeRotation(Rotation);
		const FAttachmentTransformRules InRule(EAttachmentRule::KeepWorld, false);
		Cable->AttachToComponent(RayMesh, InRule);

		ATongueProjectile* TongueCPP{
			GetWorld()->SpawnActor<ATongueProjectile>(Tongue, RayMesh->GetComponentTransform())
		};
		Cable->SetMaterial(0, TongueCPP->CableMaterial);


		Cable->SetAttachEndTo(TongueCPP, TEXT("None"));
		Cable->RegisterComponent();

		bTongueSpawned = true;
	}
	// If PowerMeter is full & PowerMode is not already active, activate PowerMode here?
}

// Called in the jump function. For every second the player holds spacebar / bumper it increases the jumpcharge by 1 to a max of 3.
void AFrogGameCharacter::JumpCharge()
{
	IsHolding = true;
}

void AFrogGameCharacter::Jumptick(float DeltaTime)
{
	JumpModifier = JumpModifier + DeltaTime;
}

void AFrogGameCharacter::FroggyJump()
{
	IsHolding = false;
	JumpBonus = JumpBonus * JumpModifier;
	BaseJump = BaseJump + JumpBonus;
	Jump();

	// Remember to reset it.
	JumpModifier = 0;
	JumpBonus = 450;
}
