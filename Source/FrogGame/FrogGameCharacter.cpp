// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "FrogGameCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/ArrowComponent.h"
#include "Engine.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "TongueProjectile.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetSystemLibrary.h"

//////////////////////////////////////////////////////////////////////////
// AFrogGameCharacter

AFrogGameCharacter::AFrogGameCharacter() {
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Set frog length
	UpdateLength();

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
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Create a spawn point for linetrace, only used to linetrace so does not need to ever be visible.
	RayMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RayMesh"));
	RayMesh->SetupAttachment(RootComponent);
	RayMesh->SetVisibility(false);

	// The half-angle of the field of view gives us two right triangles. We use the tangent of these to check objects against a cone shape rather than the box trace.
	Tangent = FMath::Tan(FollowCamera->FieldOfView / 2.0f);
	

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

//////////////////////////////////////////////////////////////////////////
// Input

void AFrogGameCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);

	// This is here just to test if raycast works, will be automatic in future
	PlayerInputComponent->BindAction("Raycast", IE_Pressed, this, &AFrogGameCharacter::AutoAim);

	// PlayerInputComponent->BindAction("Eat", IE_Pressed, this, &AFrogGameCharacter::UseTongue);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &AFrogGameCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AFrogGameCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AFrogGameCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AFrogGameCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AFrogGameCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AFrogGameCharacter::TouchStopped);
}

void AFrogGameCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	// Stuff for both Box and Line
	FHitResult OutHit;
	FVector Start = RayMesh->GetComponentLocation();
	FVector ForwardVector = RayMesh->GetForwardVector();
	FVector End = (Start + (ForwardVector * 1000.0f));
    FCollisionQueryParams CollisionParams; 

	/**
    // Stuff for BoxTrace
	float FrogRad = GetCapsuleComponent()->GetScaledCapsuleRadius();
	FVector HalfSize; // distance between the line and each side of the box. if distance is 0.5 in each direction the box will be 1x1x1.
	HalfSize.X = FrogRad, HalfSize.Y = FrogRad, HalfSize.Z = FrogRad; 
	FRotator Orientation = RayMesh->GetRelativeRotation();
	ETraceTypeQuery TraceChannel;
	bool bTraceComplex; //True to test against complex collision, false to test against simplified collision.
	const TArray<AActor*> &ActorsToIgnore;
	EDrawDebugTrace::Type DrawDebugType;
	bool bIgnoreSelf = true;
	FLinearColor TraceColor = FColor::Green;
	FLinearColor TraceHitColor = FColor::Red;
	float DrawTime; // hmm
	**/
	DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 1, 0, 1);

	bool IsHit = GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility, CollisionParams);

	if (IsHit) {
		if (OutHit.bBlockingHit) {
			if (GEngine) {
				GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("You are hitting %s"), *OutHit.GetActor()->GetName()));
			}
		}
	}
	/**
	UKismetSystemLibrary::BoxTraceSingle(GetWorld(), Start, End, HalfSize, Orientation, TraceChannel, bTraceComplex, ActorsToIgnore, DrawDebugType, OutHit, bIgnoreSelf, TraceColor, TraceHitColor, DrawTime);
	**/
}

void AFrogGameCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location) {
		Jump();
}

void AFrogGameCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location) {
		StopJumping();
}

void AFrogGameCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AFrogGameCharacter::LookUpAtRate(float Rate) {
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AFrogGameCharacter::MoveForward(float Value) {
	if ((Controller != NULL) && (Value != 0.0f)) {
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AFrogGameCharacter::MoveRight(float Value) {
	if ( (Controller != NULL) && (Value != 0.0f) ) {
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void AFrogGameCharacter::AutoAim() {
	FHitResult* HitResult = new FHitResult;
	FVector StartTrace = RayMesh->GetComponentLocation();
	FVector ForwardVector = RayMesh->GetForwardVector();
	FVector EndTrace = (StartTrace + (ForwardVector * 2000.f));
	FCollisionQueryParams* TraceParams = new FCollisionQueryParams();

	if (GetWorld()->LineTraceSingleByChannel(*HitResult, StartTrace, EndTrace, ECC_Visibility, *TraceParams)) {
		DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor(255, 0, 0), true, 1.f);

		// If hit actor is edible, display these.
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("You hit: %s"), *HitResult->Actor->GetName()));
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("%s is edible!"), *HitResult->Actor->GetName()));
		// Set hit actor to be CurrentTarget.
	}

}

void AFrogGameCharacter::UpdateLength() {
	FrogLength = GetCapsuleComponent()->GetScaledCapsuleRadius() * 2;
}	

void AFrogGameCharacter::UseTongue() {
	// Check if there is a CurrentTarget.
	// Spawn TongueProjectile Actor

	// if there is a CurrentTarget:
	// {
    // Shoot tongue at target
	// Drag Target to frog
	// Get SizePoints and PowerPoints from target.
	// Delete Target
	// Update scale (size) of frog
	// Put PowerPoints into PowerMeter
	// UpdateLength();
	// }
	
	// If there isn't a currentTarget:
	// {
    // Shoot the tongue out in a straight line until it reaches the end of the linetrace.
	// }
	
	// If PowerMeter is full & PowerMode is not already active, activate PowerMode here?
	// Destroy TongueProjectile Actor
}

FVector AFrogGameCharacter::GetEnd()
{
	FVector Temp;
	FVector Start = RayMesh->GetComponentLocation();
	FVector ForwardVector = RayMesh->GetForwardVector();
	FVector End = (Start + (ForwardVector * 1000.0f));
	Temp = End;
	return Temp;
}
