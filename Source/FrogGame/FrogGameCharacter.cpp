// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "FrogGameCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "Components/InputComponent.h"
#include "Engine.h"
#include "Engine/EngineTypes.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "DestructibleComponent.h"
#include "TongueProjectile.h"
#include "FrogGameInstance.h"
#include "CableComponent.h"
#include "Edible.h"
#include "DestructibleActor.h"

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
	GetCharacterMovement()->JumpZVelocity = BaseJump;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = BaseBoomRange; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	// Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	BoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxTrace"));
	BoxCollider->SetupAttachment(FollowCamera);
	BoxCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BoxCollider->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);
	BoxCollider->OnComponentEndOverlap.AddDynamic(this, &AFrogGameCharacter::OnBoxTraceEnd);
	// Create a spawn point for linetrace, only used to linetrace so does not need to ever be visible.
	TongueStart = GetArrowComponent();
	TongueStart->bEditableWhenInherited = true;
	TongueStart->SetupAttachment(GetMesh(), FName("head"));

	// Creates a collision sphere and attaches it to the characters right hand.
	RightHandCollision = CreateDefaultSubobject<USphereComponent>(TEXT("RightHandCollision"));
	RightHandCollision->SetupAttachment(GetMesh(), FName("hand_r"));

	// Creates a collision sphere and attaches it to the characters left hand.
	LeftHandCollision = CreateDefaultSubobject<USphereComponent>(TEXT("LeftHandCollision"));
	LeftHandCollision->SetupAttachment(GetMesh(), FName("hand_l"));

	// Setting Hud trackers to 0 at the start.
	CurrentScore = 0.f;
	CurrentPowerPoints = 0.f;

	TongueInSpeed = BaseTongueInSpeed;
	TongueOutSpeed = BaseTongueOutSpeed;
	CurrentJump = BaseJump;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

float AFrogGameCharacter::GetCurrentScore()
{
	return CurrentScore;
}

void AFrogGameCharacter::UpdateCurrentScore(float Score)
{
	CurrentScore = CurrentScore + Score;
}

float AFrogGameCharacter::GetCurrentPowerPoints()
{
	return CurrentPowerPoints;
}

void AFrogGameCharacter::UpdatePowerPoints(float Points)
{
	CurrentPowerPoints = CurrentPowerPoints + Points;
	if (CurrentPowerPoints > MaxPowerPoints)
	{
		CurrentPowerPoints = MaxPowerPoints;
	}
}

void AFrogGameCharacter::BeginPlay()
{
	Super::BeginPlay();
	const FVector Viewport{GetWorld()->GetGameViewport()->Viewport->GetSizeXY()};
	BoxCollider->SetBoxExtent(FVector(Tongue.GetDefaultObject()->TongueRange / 2.f, Viewport.X / 2.f,
	                                  Viewport.Y));
	BoxCollider->SetRelativeLocation(FVector(CameraBoom->TargetArmLength + BoxCollider->GetUnscaledBoxExtent().X, 0,
	                                         0));
}

UArrowComponent* AFrogGameCharacter::GetRayMesh()
{
	 return TongueStart;
} 

//////////////////////////////////////////////////////////////////////////
// Input

void AFrogGameCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Eat", IE_Pressed, this, &AFrogGameCharacter::Lickitung);
	PlayerInputComponent->BindAction("Punch", IE_Pressed, this, &AFrogGameCharacter::Hitmonchan);

	// This is here for test purposes, will activate when the powerbar is filled up.
	PlayerInputComponent->BindAction("PowerMode", IE_Pressed, this, &AFrogGameCharacter::PowerMode);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AFrogGameCharacter::StartJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AFrogGameCharacter::ExecuteJump);

	PlayerInputComponent->BindAction("TestSave", IE_Pressed, this, &AFrogGameCharacter::SaveGame);
	PlayerInputComponent->BindAction("TestLoad", IE_Pressed, this, &AFrogGameCharacter::LoadGame);

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
	if (bPowerMode)
	{
		PowerDrain(DeltaTime);
		if (CurrentPowerPoints <= 0)
		{
			bPowerMode = false;
			// Put in set back to frog mesh & rig here
		}
	}
	if (bIsCharging)
	{
		ChargeJump(DeltaTime);
	}
	if (bScalingUp)
	{
		UpdateCharacterScale(DeltaTime);
	}
	if (!bTongueSpawned)
	{
		AutoAim();
	}
}

// TODO: Choose target close to box origin AND player
void AFrogGameCharacter::AutoAim()
{
	TArray<AActor*> OverlappingActors;
	BoxCollider->GetOverlappingActors(OverlappingActors);
	const FVector BoxOrigin{BoxCollider->GetComponentLocation()};
	int MaxSize{SizeTier - EdibleThreshold}; // Highest size tier the player can eat
	if (MaxSize < 0)
	{
		MaxSize = 0;
	}
	for (AActor* Actor : OverlappingActors)
	{
		if (Actor == CurrentTarget && !Actor->GetComponentByClass(UDestructibleComponent::StaticClass()))
			// Don't check against itself
		{
			continue;
		}
		if (Actor->Implements<UEdible>())
		{
			// If this actor is closer to the center of the box collider than the current target is
			const float DistToActor{FVector::Dist(BoxOrigin, Actor->GetActorLocation())};
			const float DistToCurrentTarget{
				CurrentTarget ? FVector::Dist(BoxOrigin, CurrentTarget->GetActorLocation()) : 5000.f
			};
			if (DistToActor <= DistToCurrentTarget)
			{
				const FEdibleInfo SizeInfo{IEdible::Execute_GetInfo(Actor)};
				// Breaking something produces pieces two tiers smaller, so the actor must be exactly the size of the player
				ADestructibleActor* DestructibleActor{Cast<ADestructibleActor>(Actor)};
				if (DestructibleActor && BoneTarget.IsNone())
				{
					if (SizeInfo.SizeTier == SizeTier)
					{
						GetClosestChunk(DestructibleActor->GetDestructibleComponent());
						CurrentTarget = Actor;
					}
					else if (SizeInfo.SizeTier <= MaxSize)
					{
						CurrentTarget = Actor;
						BoneTarget = FName();
					}
				}
					// If the actor's size is less than or equal to the frog's size 
				else if (SizeInfo.SizeTier <= MaxSize
					&& !Actor->GetComponentByClass(UDestructibleComponent::StaticClass()))
				{
					CurrentTarget = Actor;
					BoneTarget = FName();
				}
			}
		}
	}
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

void AFrogGameCharacter::Landed(const FHitResult& Hit)
{
	GetCharacterMovement()->JumpZVelocity = CurrentJump;
}

void AFrogGameCharacter::Consume(AActor* OtherActor, const FName BoneName)
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
		float ActualSize{SizeInfo.Size};
		if (!BoneName.IsNone())
		{
			UDestructibleComponent* Destructible{
				Cast<UDestructibleComponent>(OtherActor->GetComponentByClass(UDestructibleComponent::StaticClass()))
			};
			Destructible->HideBoneByName(BoneName, PBO_Term);
			ActualSize /= SizeInfo.NumChunks; // Assuming that the actor splits into roughly same size chunks.
		}
		else
		{
			OtherActor->Destroy();
		}
		// just reset the lerp values
		bScalingUp = true;
		// We use the scaled radius value of the capsule collider to get an approximate size value for the main character.
		const float ScaledRadius{GetCapsuleComponent()->GetScaledCapsuleRadius()};
		// Compare this to the averaged bounding box size of the object being eaten and factor in the growth coefficient.
		const float SizeDiff{ActualSize / ScaledRadius * SizeInfo.GrowthCoefficient};
		// If SizeInfo.Size = 10 and ScaledRadius = 50 then we get a value of 10/50 = 0.2 or 20%.
		// Increase actor scale by this value. 
		DesiredScale = GetActorScale() * (1 + SizeDiff);
		UpdateCurrentScore(SizeInfo.ScorePoints);
		UpdatePowerPoints(SizeInfo.PowerPoints);
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

void AFrogGameCharacter::UpdateCharacterScale(const float DeltaTime)
{
	if (ScaleAlpha <= 1.0f)
	{
		ScaleAlpha += DeltaTime;
		const FVector CurrentScale{GetActorScale()};
		SetActorScale3D(FMath::Lerp(GetActorScale(), DesiredScale, ScaleAlpha));
		const float ScaleDelta{(GetActorScale() - CurrentScale).X};
		UpdateCharacterMovement(ScaleDelta);
		UpdateCameraBoom(ScaleDelta);
		UpdateAimRange();
		if (GetActorScale().X > SizeTier)
			// Frog starts at size 2, so once it hits a scale factor of 2.0+, it will increase to size 3 etc
		{
			SizeTier++;
		}
	}
	else
	{
		ScaleAlpha = 0.f;
		bScalingUp = false;
	}
}

void AFrogGameCharacter::UpdateCharacterMovement(const float ScaleDelta)
{
	UCharacterMovementComponent* Movement{GetCharacterMovement()};
	Movement->MaxWalkSpeed += BaseMaxWalkSpeed * ScaleDelta;
	CurrentJump += BaseJump * ScaleDelta;
	Movement->JumpZVelocity = CurrentJump;
	TongueInSpeed += BaseTongueInSpeed * ScaleDelta;
	TongueOutSpeed += BaseTongueOutSpeed * ScaleDelta;
}

void AFrogGameCharacter::UpdateCameraBoom(const float ScaleDelta) const
{
	// TODO: Comment this
	CameraBoom->TargetArmLength += BaseBoomRange * ScaleDelta;
	UpdateAimRange();
}

void AFrogGameCharacter::UpdateAimRange() const
{
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


		const FVector Location{TongueStart->GetComponentTransform().GetLocation()};
		const FRotator Rotation{TongueStart->GetComponentTransform().GetRotation()};
		Cable->SetRelativeLocation(Location);
		Cable->SetRelativeRotation(Rotation);
		const FAttachmentTransformRules InRule(EAttachmentRule::KeepWorld, false);
		Cable->AttachToComponent(TongueStart, InRule);
		FRotator FacingDirection{FollowCamera->GetForwardVector().ToOrientationRotator()};
		FacingDirection.Pitch = GetActorRotation().Pitch;
		SetActorRotation(FacingDirection);
		ATongueProjectile* TongueCPP{
			GetWorld()->SpawnActor<ATongueProjectile>(Tongue,
			                                          TongueStart->GetComponentTransform())
		};
		LastBone = BoneTarget;
		BoneTarget = FName();
		Cable->SetMaterial(0, TongueCPP->CableMaterial);


		Cable->SetAttachEndTo(TongueCPP, FName());
		Cable->RegisterComponent();

		bTongueSpawned = true;
	}
	// If PowerMeter is full & PowerMode is not already active, activate PowerMode here?
}

// Called in the jump function. For every second the player holds spacebar / bumper it increases the jump charge by 1 to a max of 3.
void AFrogGameCharacter::StartJump()
{
	bIsCharging = true;
}

void AFrogGameCharacter::Hitmonchan()
{
	if (bPowerMode)
	{
		// Add execution here
	}
	else
	{
		// Does nothing if player is not in power mode.
	}
}

void AFrogGameCharacter::ChargeJump(float DeltaTime)
{
	JumpModifier += (DeltaTime * ChargeSpeed); // The modifier determines what percentage of the jump bonus is applied
}

void AFrogGameCharacter::ExecuteJump()
{
	bIsCharging = false;
	JumpModifier = FMath::Clamp(JumpModifier, 0.f, 1.f);
	GetCharacterMovement()->JumpZVelocity += (JumpBonus * JumpModifier);
	//UE_LOG(LogTemp, Warning, TEXT("%f"), GetCharacterMovement()->JumpZVelocity);
	Jump();

	// Remember to reset it.
	JumpModifier = 0.f;
}

void AFrogGameCharacter::PowerMode()
{
	CurrentPowerPoints = MaxPowerPoints;
	bPowerMode = true;
	// Change from frog mesh and rig to power-frog mesh & rig here.
}

void AFrogGameCharacter::PowerDrain(float DeltaTime)
{
	const float DrainPoints = (DeltaTime * DrainSpeed);
	UpdatePowerPoints(DrainPoints);
}

void AFrogGameCharacter::OnBoxTraceEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                       UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//UE_LOG(LogTemp, Warning, TEXT("Lost sight of target!"));

	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		if (OtherActor == CurrentTarget)
		{
			CurrentTarget = nullptr;
		}
	}
}

void AFrogGameCharacter::SaveGame()
{
	UFrogGameInstance* GameInstance{Cast<UFrogGameInstance>(GetGameInstance())};
	GameInstance->SaveCurrentToSlot();
}

void AFrogGameCharacter::LoadGame()
{
	Cast<UFrogGameInstance>(GetGameInstance())->LoadCheckpoint();
}

void AFrogGameCharacter::GetClosestChunk(UDestructibleComponent* Component)
{
	FName ClosestBone;
	float DistToBone{5000.f};
	for (const auto Bone : Component->GetAllSocketNames())
	{
		if (Component->IsBoneHiddenByName(Bone) || Bone == TEXT("Root"))
		{
			continue;
		}
		const float Distance{FVector::Dist(Component->GetBoneLocation(Bone), GetActorLocation())};
		if (Distance < DistToBone)
		{
			ClosestBone = Bone;
			DistToBone = Distance;
		}
	}
	if (ClosestBone == LastBone)
	{
		return;
	}
	BoneTarget = ClosestBone;
	UE_LOG(LogTemp, Warning, TEXT("%s"), *BoneTarget.ToString());
}
