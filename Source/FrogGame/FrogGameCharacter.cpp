// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "FrogGameCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "Components/InputComponent.h"
#include "Engine.h"
#include "Engine/EngineTypes.h"
#include "TargetingReticle.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "TongueProjectile.h"
#include "FrogGameInstance.h"
#include "Edible.h"
#include "CableComponent.h"

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
	CameraBoom->TargetArmLength = 500.f;
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
	BoxCollider->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Ignore);
	BoxCollider->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);

	BoxCollider->OnComponentEndOverlap.AddDynamic(this, &AFrogGameCharacter::OnBoxTraceEnd);
	// We use the arrow component as spawn point for the tongue and attach it to the head bone
	TongueStart = GetArrowComponent();
	TongueStart->bEditableWhenInherited = true;
	TongueStart->SetupAttachment(GetMesh(), FName("head"));


	// Creates a collision sphere and attaches it to the characters right hand.
	RightHandCollision = CreateDefaultSubobject<USphereComponent>(TEXT("RightHandCollision"));
	RightHandCollision->SetupAttachment(GetMesh(), FName("hand_r"));
	RightHandCollision->SetNotifyRigidBodyCollision(true);

	// Creates a collision sphere and attaches it to the characters left hand.
	LeftHandCollision = CreateDefaultSubobject<USphereComponent>(TEXT("LeftHandCollision"));
	LeftHandCollision->SetupAttachment(GetMesh(), FName("hand_l"));
	LeftHandCollision->SetNotifyRigidBodyCollision(true);
	SetHandCollision(RightHandCollision, TEXT("NoCollision"));
	SetHandCollision(LeftHandCollision, TEXT("NoCollision"));
	RightHandCollision->SetCollisionObjectType(ECC_WorldDynamic);
	LeftHandCollision->SetCollisionObjectType(ECC_WorldDynamic);
}

float AFrogGameCharacter::GetCurrentScore()
{
	return CurrentScore;
}

uint8 AFrogGameCharacter::GetCurrentSizeTier()
{
	return SizeTier;
}

uint8 AFrogGameCharacter::GetNextSizeTier()
{
	return SizeTier + 1;
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
	if (TongueBP)
	{
		const FVector Viewport{GetWorld()->GetGameViewport()->Viewport->GetSizeXY()};
		BoxCollider->SetBoxExtent(FVector(TongueBP.GetDefaultObject()->TongueRange / 2.f, Viewport.X / 2.f,
		                                  Viewport.Y));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Missing reference to TongueProjectile blueprint!"));
	}
	BoxCollider->SetRelativeLocation(FVector(CameraBoom->TargetArmLength + BoxCollider->GetUnscaledBoxExtent().X, 0,
	                                         0));
	BaseBoomRange = CameraBoom->TargetArmLength;
	LeftHandCollision->OnComponentHit.AddDynamic(this, &AFrogGameCharacter::OnAttackHit);
	RightHandCollision->OnComponentHit.AddDynamic(this, &AFrogGameCharacter::OnAttackHit);
	MaxAngleRadians = FMath::DegreesToRadians(MaxAngle);

	// Setting Hud trackers to 0 at the start.
	CurrentScore = 0.f;
	CurrentPowerPoints = 0.f;

	TongueReturnSpeed = BaseTongueReturnSpeed;
	TongueSeekSpeed = BaseTongueSeekSpeed;
	CurrentJump = BaseJump;
}

UArrowComponent* AFrogGameCharacter::GetTongueStart()
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
	PlayerInputComponent->BindAction("ClearTarget", IE_Pressed, this, &AFrogGameCharacter::ClearCurrentTarget);
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

void AFrogGameCharacter::ClearCurrentTarget()
{
	CurrentTarget = nullptr;
	Targets.RemoveAt(0);
	CurrentTargetScore = 0.0f;
}

void AFrogGameCharacter::DeactivatePowerMode()
{
	bPowerMode = false;
	SetHandCollision(RightHandCollision, TEXT("NoCollision"));
	SetHandCollision(LeftHandCollision, TEXT("NoCollision"));
	// Put in set back to frog mesh & rig here
}

void AFrogGameCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bPowerMode)
	{
		PowerDrain(DeltaTime);
		if (CurrentPowerPoints <= 0)
		{
			DeactivatePowerMode();
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

	AutoAim();
}

// TODO: Choose target close to box origin AND player
void AFrogGameCharacter::AutoAim()
{
	TArray<AActor*> OverlappingActors;
	// Necessary to update the auto-aim volume whenever the camera is turned. Might be expensive?
	BoxCollider->UpdateOverlapsImpl();
	BoxCollider->GetOverlappingActors(OverlappingActors);
	if (bPowerMode)
	{
		Targets = OverlappingActors; // Remember to clear this when exiting Power mode
		return;
	}
	const int MaxSize{SizeTier - EdibleThreshold}; // Highest size tier the player can eat
	for (AActor* Actor : OverlappingActors)
	{
		if (Actor == CurrentTarget) // Don't check against itself	
		{
			continue;
		}
		if (Actor->Implements<UEdible>())
		{
			const FEdibleInfo SizeInfo{IEdible::Execute_GetInfo(Actor)};
			if (SizeInfo.SizeTier > MaxSize) // Object is too big, ignore.
			{
				continue;
			}
			const float DistToActor{FVector::Dist(GetActorLocation(), Actor->GetActorLocation())};
			// We use the box collider forward vector multiplied by twice the extent of the box.
			const FVector MaxPointInBox{BoxCollider->GetForwardVector() * (BoxCollider->GetUnscaledBoxExtent().X * 2.f)};
			const float DistToMaxRange{FVector::Dist(GetActorLocation(), GetActorLocation() + MaxPointInBox)};
			// How ideal this actor is to become the current target based on distance.
			const float DistanceScore{1.f - FMath::Clamp(DistToActor / DistToMaxRange, 0.f, MaxDistanceScore)};
			FVector V1{FollowCamera->GetForwardVector()};
			// Get a line from camera to actor
			FVector V2{FollowCamera->GetComponentLocation() - Actor->GetActorLocation()};
			V2.Normalize();
			float AngleRad{FMath::Acos(FVector::DotProduct(V1, V2))};
			// We need another dot product to figure out our winding
			// in case the angle is something like 480 or -900 we normalize it to +-PI
			if (FVector::DotProduct(FollowCamera->GetRightVector(), V2) < 0.f)
			{
				AngleRad = PI * 2.f - AngleRad;
			}
			AngleRad = FMath::Abs(FMath::Clamp(AngleRad - PI, -PI, PI));
			// How ideal this actor is to become the current target based on proximity to the middle of the camera view. A lower value is better.
			// An angle exactly as large as the max angle will result in (X / X) * MaxAngleScore = MaxAngleScore.
			const float AngleScore = FMath::Clamp(AngleRad / MaxAngleRadians, 0.f, MaxAngleScore);
			const float TotalScore{DistanceScore - AngleScore};
			if (TotalScore > CurrentTargetScore + AimStickiness)
			{
				CurrentTarget = Actor;
				Targets.Add(CurrentTarget);
				UE_LOG(LogTemp, Warning,
				       TEXT("Current Target is: %s, with a distance score of: %f and angle score of: %f"),
				       *CurrentTarget->GetName(), DistanceScore, AngleScore)
				UE_LOG(LogTemp, Warning, TEXT("Total Score: %f, vs Last Score: %f"), TotalScore,
				       CurrentTargetScore)

				CurrentTargetScore = TotalScore;
			}
		}
	}
	if (Targets.Num() > 0)
	{
		SpawnTargetingMesh();
	}
}

void AFrogGameCharacter::SpawnTargetingMesh()
{
	for (auto Target : Targets)
	{
		// Temp
		UKismetSystemLibrary::DrawDebugArrow(
			GetWorld(),
			BoxCollider->GetComponentLocation() - BoxCollider->GetForwardVector() * BoxCollider
			                                                                        ->GetUnscaledBoxExtent().X,
			Target->GetActorLocation(), 3.f,
			FLinearColor::Red);
		if (TargetingMesh && !bTongueSpawned)
		{
			UE_LOG(LogTemp, Warning, TEXT("Test"))
			FVector TargetToPlayer{
				IEdible::Execute_GetTargetComponent(Target)->GetComponentLocation() - FollowCamera->
				GetComponentLocation()
			};
			TargetToPlayer.Normalize();
			const FRotator RotationToPlayer{TargetToPlayer.ToOrientationRotator()};

			auto TargetingReticule{IEdible::Execute_GetTargetingReticule(Target)};
			if (TargetingReticule)
			{
				TargetingReticule->DrawReticle(TargetToPlayer, RotationToPlayer, 1.f);
				// Normalize the line from A to B, multiply with desired distance from Target
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
		const FVector PrevScale{GetActorScale()};
		const FVector DesiredScale{GetActorScale() + ExtraScaleTotal};
		SetActorScale3D(FMath::Lerp(GetActorScale(), DesiredScale, ScaleAlpha));
		const float ScaleDelta{(GetActorScale() - PrevScale).X};
		ExtraScaleTotal -= FVector(ScaleDelta);
		UpdateCharacterMovement(ScaleDelta);
		UpdateCameraBoom(ScaleDelta);
		UpdateAimRange();
		CurrentCableWidth += BaseCableWidth * ScaleDelta;

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
	TongueReturnSpeed += BaseTongueReturnSpeed * ScaleDelta;
	TongueSeekSpeed += BaseTongueSeekSpeed * ScaleDelta;
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
	Extent.X = (TongueBP.GetDefaultObject()->TongueRange / 2.f) * GetActorScale().X;
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
		NumTongues = 0;
		UE_LOG(LogTemp, Warning, TEXT("Num Tongues left: %d, Num Targets: %d"), NumTongues, Targets.Num());
		if (Targets.Num() == 0)
		{
			ATongueProjectile* TongueCPP{
				GetWorld()->SpawnActor<ATongueProjectile>(TongueBP,
				                                          TongueStart->
				                                          GetComponentTransform())
			};
			NumTongues++;
			TongueCPP->ActivateTongue(nullptr);
		}
		else
		{
			for (auto Target : Targets)
			{
				if (Target->Implements<UEdible>())
				{
					IEdible::Execute_GetTargetingReticule(Target)->SetHiddenInGame(true);
					ATongueProjectile* TongueCPP{
						GetWorld()->SpawnActor<ATongueProjectile>(TongueBP,
						                                          TongueStart->
						                                          GetComponentTransform())
					};
					if (Targets.Num() == 1)
					{
						FRotator FacingDirection{FollowCamera->GetForwardVector().ToOrientationRotator()};
						FacingDirection.Pitch = GetActorRotation().Pitch;
						SetActorRotation(FacingDirection);
					}
					NumTongues++;
					TongueCPP->ActivateTongue(Target);
				}
			}
		}
		bTongueSpawned = true;
	}
	// If PowerMeter is full & PowerMode is not already active, activate PowerMode here?
}

void AFrogGameCharacter::Consume(AActor* OtherActor, ATongueProjectile* Tongue)
{
	if (Tongue->Cable)
	{
		Tongue->Cable->DestroyComponent();
		Tongue->Destroy();
		UE_LOG(LogTemp, Warning, TEXT("Destroying Tongue"));

		NumTongues--;

		if (NumTongues == 0)
		{
			bTongueSpawned = false;
		}
	}
	if (!OtherActor)
	{
		return;
	}
	if (OtherActor->Implements<UEdible>())
	{
		const FEdibleInfo SizeInfo{IEdible::Execute_GetInfo(OtherActor)};

		OtherActor->Destroy();

		// just reset the lerp values
		bScalingUp = true;
		// We use the scaled radius value of the capsule collider to get an approximate size value for the main character.
		const float ScaledRadius{GetCapsuleComponent()->GetScaledCapsuleRadius()};
		// Compare this to the averaged bounding box size of the object being eaten and factor in the growth coefficient.
		const float SizeDiff{SizeInfo.Size / ScaledRadius * SizeInfo.GrowthCoefficient};
		// If SizeInfo.Size = 10 and ScaledRadius = 50 then we get a value of 10/50 = 0.2 or 20%.
		// Increase actor scale by this value. 
		const FVector ScaleVector = GetActorScale() * (1 + SizeDiff);
		ExtraScaleTotal += (ScaleVector - GetActorScale());
		UpdateCurrentScore(SizeInfo.ScorePoints);
		UpdatePowerPoints(SizeInfo.PowerPoints);
	}
}

// Called in the jump function. For every second the player holds spacebar / bumper it increases the jump charge by 1 to a max of 3.
void AFrogGameCharacter::StartJump()
{
	bIsCharging = true;
}

void AFrogGameCharacter::SetHandCollision(USphereComponent* Collider, FName CollisionProfile)
{
	Collider->SetCollisionProfileName(CollisionProfile);
	Collider->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	if (CollisionProfile == TEXT("Pawn"))
	{
		Collider->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		Collider->SetSimulatePhysics(true);
	}
	else

	{
		Collider->SetSimulatePhysics(false);
	}
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

void AFrogGameCharacter::OnAttackHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                     FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor != this && OtherComp != nullptr && OtherActor->Implements<UEdible>())
	{
		UE_LOG(LogTemp, Warning, TEXT("Hit Event with %s!"), *OtherActor->GetName())


		//if (Destructible)
		//{
		TSubclassOf<UDamageType> const ValidDamageTypeClass = TSubclassOf<UDamageType>(UDamageType::StaticClass());
		FDamageEvent DamageEvent(ValidDamageTypeClass);
		//Destructible->TakeDamage(PunchDamage, DamageEvent, GetController(), this);
		//}
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
	SetHandCollision(RightHandCollision, TEXT("Pawn"));
	SetHandCollision(LeftHandCollision, TEXT("Pawn"));
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
			ClearCurrentTarget();
			UE_LOG(LogTemp, Warning, TEXT("Target stopped overlapping."))
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
