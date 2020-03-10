// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "FrogGameCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "Components/InputComponent.h"
#include "Engine.h"
#include "Engine/EngineTypes.h"
#include "TargetingReticule.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "TongueProjectile.h"
#include "FrogGameInstance.h"
#include "Edible.h"
#include "CableComponent.h"
#include "BaseEdible.h"
#include "TonguePivot.h"

//////////////////////////////////////////////////////////////////////////
// AFrogGameCharacter

AFrogGameCharacter::AFrogGameCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(13.f, 13.0f);
	FVector2D Capsule;
	GetCapsuleComponent()->GetUnscaledCapsuleSize(Capsule.X, Capsule.Y);
	NeutralModeSettings.CapsuleSize = Capsule;
	PowerModeSettings.CapsuleSize = FVector2D(32.f, 75.0f);

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
	GetCharacterMovement()->GravityScale = 3.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 200.f;
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	// Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	AutoAimVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxTrace"));
	AutoAimVolume->SetupAttachment(FollowCamera);
	AutoAimVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	AutoAimVolume->SetCollisionProfileName(TEXT("AutoAim"));
	AutoAimVolume->OnComponentEndOverlap.AddDynamic(this, &AFrogGameCharacter::OnBoxTraceEnd);

	// We use the arrow component as spawn point for the tongue and attach it to the head bone
	TongueStart = GetArrowComponent();
	TongueStart->bEditableWhenInherited = true;
	TongueStart->SetupAttachment(GetMesh(), NeutralModeSettings.TongueBoneTarget);
	static ConstructorHelpers::FClassFinder<ATongueProjectile> TongueProjectileBP(
		TEXT("/Game/Blueprints/Characters/Main_Character/BP_TongueProjectile"));
	if (TongueProjectileBP.Class != nullptr)
	{
		TongueBP = TongueProjectileBP.Class;
	}
	// Power mode punch volumes need to be changed once we get the correct mesh
	// Creates a collision sphere and attaches it to the characters right hand.
	RightHandCollision = CreateDefaultSubobject<USphereComponent>(TEXT("RightHandCollision"));
	SetHandCollision(RightHandCollision, TEXT("NoCollision"));
	RightHandCollision->CanCharacterStepUpOn = ECB_No;
	RightHandCollision->InitSphereRadius(6.f);
	RightHandCollision->SetRelativeLocation(FVector(0.f,9.f,0.f));
	// Creates a collision sphere and attaches it to the characters left hand.
	LeftHandCollision = CreateDefaultSubobject<USphereComponent>(TEXT("LeftHandCollision"));
	SetHandCollision(LeftHandCollision, TEXT("NoCollision"));
	LeftHandCollision->CanCharacterStepUpOn = ECB_No;
	LeftHandCollision->InitSphereRadius(6.f);
	LeftHandCollision->SetRelativeLocation(FVector(0.f,-9.f,0.f));
	PowerModeSettings.BaseBoomRange = 500.f;
	
	PowerModeSettings.TongueBoneTarget = TEXT("head_j");
}

void AFrogGameCharacter::SetHandCollision(USphereComponent* Collider, FName CollisionProfile)
{
	Collider->SetCollisionProfileName(CollisionProfile);
	if (CollisionProfile == TEXT("Punch"))
	{
		Collider->SetNotifyRigidBodyCollision(true);
	}
	else
	{
		Collider->SetNotifyRigidBodyCollision(false);
	}
}

void AFrogGameCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (TongueBP)
	{
		const FVector Viewport{GetWorld()->GetGameViewport()->Viewport->GetSizeXY()};
		AutoAimVolume->SetBoxExtent(FVector(TongueBP.GetDefaultObject()->TongueRange / 2.f, Viewport.X / 2.f,
		                                    Viewport.Y));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Missing reference to TongueProjectile blueprint!"));
	}
	if (GetMesh()->SkeletalMesh)
	{
		NeutralModeSettings.Mesh = GetMesh()->SkeletalMesh;
		if (const auto AnimInstance = GetMesh()->GetAnimInstance()->GetClass())
		{
			NeutralModeSettings.AnimBP = AnimInstance;
		}
	}
	AutoAimVolume->SetRelativeLocation(FVector(CameraBoom->TargetArmLength + AutoAimVolume->GetUnscaledBoxExtent().X, 0,
	                                           0));
	BaseBoomRange = CameraBoom->TargetArmLength / GetActorScale().X;
	LeftHandCollision->OnComponentHit.AddDynamic(this, &AFrogGameCharacter::OnAttackHit);
	RightHandCollision->OnComponentHit.AddDynamic(this, &AFrogGameCharacter::OnAttackHit);
	MaxAngleRadians = FMath::DegreesToRadians(MaxAngle);

	// Setting Hud trackers to 0 at the start.
	CurrentScore = 0.f;
	CurrentPowerPoints = 0.f;

	TongueReturnSpeed = BaseTongueReturnSpeed;
	TongueSeekSpeed = BaseTongueSeekSpeed;
	CurrentCableWidth = BaseCableWidth;
	CurrentJump = BaseJump;
	SizeTier = 1 + GetActorScale().X;
	GetCharacterMovement()->GravityScale = NeutralModeSettings.GravityScale;
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

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	//PlayerInputComponent->BindAction("Jump", IE_Released, this, &AFrogGameCharacter::ExecuteJump);

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


void AFrogGameCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	PositionAimBox();
	if (bPowerMode)
	{
		if (Targets.Num() > 0)
		{
			SpawnTargetingMesh(Targets);
		}
		if (!bTargetExists)
		{
			AutoAim();
		}
		else if (bChargingEat)
		{
			EatCharge += DeltaTime;
			if (EatCharge >= EatChargeTime)
			{
				EatCharge = 0.f;
				bTargetExists = false;
			}
		}
		PowerDrain(DeltaTime);
		if (CurrentPowerPoints <= 0)
		{
			DeactivatePowerMode();
		}
	}
	if (bScalingUp)
	{
		UpdateCharacterScale(DeltaTime);
	}
	if (!bTongueSpawned && !bPowerMode)
	{
		AutoAim();
	}
}

// TODO: Choose target close to box origin AND player
void AFrogGameCharacter::AutoAim()
{
	TArray<AActor*> OverlappingActors;
	// Necessary to update the auto-aim volume whenever the camera is turned. Might be expensive?
	AutoAimVolume->UpdateOverlapsImpl();
	AutoAimVolume->GetOverlappingActors(OverlappingActors);
	if (bPowerMode)
	{
		Targets = OverlappingActors; // Remember to clear this when exiting Power mode
	}
	else
	{
		for (AActor* Actor : OverlappingActors)
		{
			if (Actor == CurrentTarget) // Don't check against itself	
			{
				continue;
			}
			if (Actor->Implements<UEdible>())
			{
				const float TotalScore{GetTotalScore(Actor)};
				if (TotalScore > CurrentTargetScore + AimStickiness)
				{
					CurrentTarget = Actor;
					/*UE_LOG(LogTemp, Warning,
					       TEXT("Current Target is: %s, with a distance score of: %f and angle score of: %f"),
					       *CurrentTarget->GetName(), DistanceScore, AngleScore)
					UE_LOG(LogTemp, Warning, TEXT("Total Score: %f, vs Last Score: %f"), TotalScore,
					       CurrentTargetScore)*/

					CurrentTargetScore = TotalScore;
				}
			}
		}
	}
	if (CurrentTarget)
	{
		SpawnTargetingMesh(TArray<AActor*>{CurrentTarget});
	}
	if (Targets.Num() > 0)
	{
		SpawnTargetingMesh(Targets);
	}
}

float AFrogGameCharacter::CalcDistanceScore(AActor* Actor) const
{
	const float DistToActor{FVector::Dist(GetActorLocation(), Actor->GetActorLocation())};
	// We use the box collider forward vector multiplied by twice the extent of the box.
	const FVector MaxPointInBox{
		AutoAimVolume->GetForwardVector() * (AutoAimVolume->GetUnscaledBoxExtent().X * 2.f)
	};
	const float DistToMaxRange{FVector::Dist(GetActorLocation(), GetActorLocation() + MaxPointInBox)};
	// How ideal this actor is to become the current target based on distance.
	return {1.f - FMath::Clamp(DistToActor / DistToMaxRange, 0.f, MaxDistanceScore)};
}

float AFrogGameCharacter::CalcAngleScore(AActor* Actor) const
{
	const FVector V1{FollowCamera->GetForwardVector()};
	// Get a line from camera to actor
	FVector V2{(FollowCamera->GetComponentLocation() - Actor->GetActorLocation())};
	V2.Normalize();
	float AngleRad{FMath::Acos(FVector::DotProduct(V1, V2))};
	// We need another dot product to figure out our winding
	// in case the angle is something like 480 or -900 we normalize it to +-PI
	if (FVector::DotProduct(FollowCamera->GetRightVector(), V2) < 0.f)
	{
		AngleRad = PI * 2.f - AngleRad;
	}
	AngleRad = FMath::Abs(FMath::Clamp(AngleRad - PI, -PI, PI));

	return {FMath::Clamp(AngleRad / MaxAngleRadians, 0.f, MaxAngleScore)};
}

float AFrogGameCharacter::GetTotalScore(AActor* Actor) const
{
	const int MaxSize{SizeTier - EdibleThreshold}; // Highest size tier the player can eat
	const FEdibleInfo SizeInfo{IEdible::Execute_GetInfo(Actor)};
	if (SizeInfo.SizeTier > MaxSize) // Object is too big, ignore.
	{
		return 0.f;
	}
	const float DistanceScore{CalcDistanceScore(Actor)};
	const float AngleScore{CalcAngleScore(Actor)};
	return {DistanceScore - AngleScore};
}

void AFrogGameCharacter::SpawnTargetingMesh(const TArray<AActor*>& TargetEdibles) const
{
	for (auto Target : TargetEdibles)
	{
		// Temp
		//UKismetSystemLibrary::DrawDebugArrow(
		//	GetWorld(),
		//	AutoAimVolume->GetComponentLocation() - AutoAimVolume->GetForwardVector() * AutoAimVolume
		//	                                                                        ->GetUnscaledBoxExtent().X,
		//	Target->GetActorLocation(), 3.f,
		//	FLinearColor::Red);
		auto TargetingReticule{IEdible::Execute_GetTargetingReticule(Target)};
		if (TargetingReticule)
		{
			FVector TargetToPlayer{
				FollowCamera->GetComponentLocation() - IEdible::Execute_GetTargetComponent(Target)->
				GetComponentLocation()
			};
			TargetToPlayer.Normalize();
			const int TierDiff{SizeTier - IEdible::Execute_GetInfo(Target).SizeTier};
			// Temp solution
			float Size = 75.f;
			if (TierDiff == 2)
			{
				Size = 50.f;
			}
			else if (TierDiff > 2)
			{
				Size = 25.f;
			}
			TargetingReticule->DrawReticule(TargetToPlayer, 0.05f, Size);
			// Normalize the line from A to B, multiply with desired distance from Target
		}
	}
}

void AFrogGameCharacter::ClearCurrentTarget()
{
	CurrentTarget = nullptr;
	Targets.Empty();
	CurrentTargetScore = 0.0f;
}


void AFrogGameCharacter::PositionAimBox()
{
	AutoAimVolume->SetWorldLocation(
		GetActorLocation() + FollowCamera->GetForwardVector() * AutoAimVolume->GetScaledBoxExtent().X);
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


void AFrogGameCharacter::Landed(const FHitResult& Hit)
{
	GetCharacterMovement()->JumpZVelocity = CurrentJump;
}


void AFrogGameCharacter::UpdateCharacterScale(const float DeltaTime)
{
	if (ScaleAlpha <= 1.0f)
	{
		ScaleAlpha += DeltaTime;
		const FVector PrevScale{GetActorScale()};
		const FVector DesiredScale{GetActorScale() + FVector(ExtraScaleBank)};
		SetActorScale3D(FMath::Lerp(GetActorScale(), DesiredScale, ScaleAlpha));
		const float ScaleDelta{(GetActorScale() - PrevScale).X};
		ExtraScaleBank -= ScaleDelta;
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
	Movement->MaxWalkSpeed += BaseMaxWalkSpeed * (ScaleDelta * 0.5f);
	CurrentJump += BaseJump * (ScaleDelta * 0.2f); // 0.2f is temporary to reduce jump height increase
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
	FVector Extent{AutoAimVolume->GetUnscaledBoxExtent()};
	Extent.X = (TongueBP.GetDefaultObject()->TongueRange / 2.f) * GetActorScale().X;
	const float XDiff{Extent.X - AutoAimVolume->GetUnscaledBoxExtent().X};
	AutoAimVolume->SetBoxExtent(Extent);
	//FVector NewPosition{AutoAimVolume->GetRelativeLocation()};
	//NewPosition.X += XDiff;
	//AutoAimVolume->SetRelativeLocation(NewPosition);
}


void AFrogGameCharacter::Lickitung()
{
	// TODO: When activating this, set the target's collision channel to a custom one that only that object has. 
	if (!bTongueSpawned)
	{
		TArray<AActor*> Edibles = Targets;
		NumTongues = 0;
		if (Edibles.Num() == 0 && !CurrentTarget)
		{
			SpawnTongue(nullptr);
		}
		else if (CurrentTarget)
		{
			SpawnTongue(CurrentTarget);
			// Additionally face the target 
			FRotator FacingDirection{FollowCamera->GetForwardVector().ToOrientationRotator()};
			FacingDirection.Pitch = GetActorRotation().Pitch;
			SetActorRotation(FacingDirection);
		}
		else
		{
			while(Edibles.Num() > 5)
			{
				Edibles.Pop();
			}
			for (auto Target : Edibles)
			{
				if (Target->Implements<UEdible>())
				{
					SpawnTongue(Target);
				}
			}
		}
		bTongueSpawned = true;
	}
}


void AFrogGameCharacter::SpawnTongue(AActor* Target)
{
	ATongueProjectile* TongueCPP{
		GetWorld()->SpawnActor<ATongueProjectile>(TongueBP,
		                                          TongueStart->
		                                          GetComponentTransform())
	};
	NumTongues++;
	TongueCPP->ActivateTongue(Target);
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

		IncreaseScale(SizeInfo);
	}
}

void AFrogGameCharacter::IncreaseScale(const FEdibleInfo SizeInfo)
{
	// just reset the lerp values
	bScalingUp = true;
	// We use the scaled radius value of the capsule collider to get an approximate size value for the main character.
	const float ScaledRadius{GetCapsuleComponent()->GetScaledCapsuleRadius()};
	// Compare this to the averaged bounding box size of the object being eaten and factor in the growth coefficient.
	// If SizeInfo.Size = 10 and ScaledRadius = 50 then we get a value of 10/50 = 0.2 or 20%.
	// Increase actor scale by this value. 
	const float SizeDiff{SizeInfo.Size * SizeInfo.GrowthCoefficient / ScaledRadius};
	ExtraScaleBank += SizeDiff;
	//UE_LOG(LogTemp, Warning, TEXT("SizeDiff = %f"), SizeDiff)

	UpdateCurrentScore(SizeInfo.ScorePoints);
	UpdatePowerPoints(SizeInfo.PowerPoints);
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
		// For later, when we have a proper punch animation, turn on collision only when the punch of the player is at the end point
		// Also need to make sure the animation cancels early if it hits something, or turns off collision once it does, so it doesn't keep adding collision events.
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AFrogGameCharacter::RemoveHandCollision, 0.5f,
		                                       false);
		SetHandCollision(RightHandCollision, TEXT("Punch"));
		SetHandCollision(LeftHandCollision, TEXT("Punch"));
		// Add execution here
	}
	else
	{
		// Does nothing if player is not in power mode.
	}
}

void AFrogGameCharacter::RemoveHandCollision()
{
	SetHandCollision(RightHandCollision, TEXT("NoCollision"));
	SetHandCollision(LeftHandCollision, TEXT("NoCollision"));
}

void AFrogGameCharacter::OnAttackHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                     FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor != this && OtherComp != nullptr && OtherActor->Implements<UEdible>())
	{
		UE_LOG(LogTemp, Warning, TEXT("Hit Event with %s!"), *OtherActor->GetName())
		RemoveHandCollision();
		ABaseEdible* Destructible{Cast<ABaseEdible>(OtherActor)};
		if (Destructible)
		{
			TSubclassOf<UDamageType> const ValidDamageTypeClass = TSubclassOf<UDamageType>(UDamageType::StaticClass());
			const FDamageEvent DamageEvent(ValidDamageTypeClass);
			Destructible->TakeDamage(PunchDamage, DamageEvent, GetController(), this);
		}
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
	CurrentTarget = nullptr;
	SetPlayerModel(PowerModeSettings);
	const FAttachmentTransformRules InRule{EAttachmentRule::KeepRelative, false};

	RightHandCollision->AttachToComponent(GetMesh(),InRule,  TEXT("r_wrist_j"));
	LeftHandCollision->AttachToComponent(GetMesh(),InRule, TEXT("l_wrist_j"));
	// Change from frog mesh and rig to power-frog mesh & rig here.
}

void AFrogGameCharacter::SetPlayerModel(const FCharacterSettings& CharacterSettings)
{
	GetMesh()->SetAnimInstanceClass(CharacterSettings.AnimBP);
	GetMesh()->SetSkeletalMesh(CharacterSettings.Mesh);

	GetCapsuleComponent()->SetCapsuleSize(CharacterSettings.CapsuleSize.X, CharacterSettings.CapsuleSize.Y);
	const FVector Offset{0, 0, -CharacterSettings.CapsuleSize.Y};
	// not sure if this will be correct when more scale is applied.
	GetMesh()->SetRelativeLocation(Offset);
	CameraBoom->TargetArmLength = CharacterSettings.BaseBoomRange * GetActorScale().X;
	GetCharacterMovement()->MaxWalkSpeed = CharacterSettings.BaseWalkSpeed * GetActorScale().X;
	const FAttachmentTransformRules InRule{EAttachmentRule::SnapToTarget, false};
	TongueStart->AttachToComponent(GetMesh(), InRule, CharacterSettings.TongueBoneTarget);
	GetCharacterMovement()->GravityScale = CharacterSettings.GravityScale;

	// Note: Stuff like Cable width and the size of the nodule at the end of the tongue not set right now.
}

void AFrogGameCharacter::DeactivatePowerMode()
{
	bPowerMode = false;
	SetPlayerModel(NeutralModeSettings);
	SetHandCollision(RightHandCollision, TEXT("NoCollision"));
	SetHandCollision(LeftHandCollision, TEXT("NoCollision"));
}

void AFrogGameCharacter::UpdatePowerPoints(const float Points)
{
	CurrentPowerPoints = CurrentPowerPoints + Points;
	if (CurrentPowerPoints >= MaxPowerPoints)
	{
		CurrentPowerPoints = MaxPowerPoints;
		PowerMode();
	}
}

void AFrogGameCharacter::PowerDrain(const float DeltaTime)
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
		if (OtherActor == CurrentTarget && !bPowerMode)
		{
			ClearCurrentTarget();
			UE_LOG(LogTemp, Warning, TEXT("Target stopped overlapping."))
		}
		if (bPowerMode)
		{
			int Index{0};
			for (auto Target : Targets)
			{
				if (Target == OtherActor)
				{
					Targets.RemoveAt(Index);
					return;
				}
				Index++;
			}
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
