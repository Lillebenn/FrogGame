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
#include "FrogGameInstance.h"
#include "Edible.h"
#include "EdibleObject.h"
#include "SimpleCreature.h"

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
	WhirlwindVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxTrace"));
	WhirlwindVolume->SetupAttachment(RootComponent);
	WhirlwindVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	WhirlwindVolume->SetCollisionProfileName(TEXT("Whirlwind"));
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


	// We use the arrow component as spawn point for the tongue and attach it to the head bone
	TongueStart = GetArrowComponent();
	TongueStart->bEditableWhenInherited = true;
	TongueStart->SetupAttachment(GetMesh(), NeutralModeSettings.HeadSocket);

	// Power mode punch volumes need to be changed once we get the correct mesh
	// Creates a collision sphere and attaches it to the characters right hand.
	RightHandCollision = CreateDefaultSubobject<USphereComponent>(TEXT("RightHandCollision"));
	SetHandCollision(RightHandCollision, TEXT("NoCollision"));
	RightHandCollision->CanCharacterStepUpOn = ECB_No;
	RightHandCollision->InitSphereRadius(6.f);
	RightHandCollision->SetRelativeLocation(FVector(0.f, 9.f, 0.f));
	// Creates a collision sphere and attaches it to the characters left hand.
	LeftHandCollision = CreateDefaultSubobject<USphereComponent>(TEXT("LeftHandCollision"));
	SetHandCollision(LeftHandCollision, TEXT("NoCollision"));
	LeftHandCollision->CanCharacterStepUpOn = ECB_No;
	LeftHandCollision->InitSphereRadius(6.f);
	LeftHandCollision->SetRelativeLocation(FVector(0.f, -9.f, 0.f));
	PowerModeSettings.BaseBoomRange = 500.f;
}

void AFrogGameCharacter::SetHandCollision(USphereComponent* Collider, FName CollisionProfile)
{
	Collider->SetCollisionProfileName(CollisionProfile);
	Collider->SetCollisionObjectType(ECC_GameTraceChannel3);
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


	WhirlwindVolume->OnComponentBeginOverlap.AddDynamic(this, &AFrogGameCharacter::OnWhirlwindBeginOverlap);
	WhirlwindVolume->OnComponentEndOverlap.AddDynamic(this, &AFrogGameCharacter::OnWhirlwindEndOverlap);
	BaseBoomRange = CameraBoom->TargetArmLength / GetActorScale().X;
	LeftHandCollision->OnComponentBeginOverlap.AddDynamic(this, &AFrogGameCharacter::OnAttackOverlap);
	RightHandCollision->OnComponentBeginOverlap.AddDynamic(this, &AFrogGameCharacter::OnAttackOverlap);


	// Setting Hud trackers to 0 at the start.
	CurrentScore = 0.f;
	CurrentPowerPoints = 0.f;
	BaseJump = NeutralModeSettings.BaseJumpZHeight;
	CurrentJump = BaseJump;
	SizeTier = 1 + GetActorScale().X;
	BaseMaxWalkSpeed = NeutralModeSettings.BaseWalkSpeed;
	GetCharacterMovement()->GravityScale = NeutralModeSettings.GravityScale;
	GetCharacterMovement()->MaxWalkSpeed = BaseMaxWalkSpeed;

	// Setup the default whirlwind swirl settings
	DefaultWhirlwindSwirl.DefaultLinearUpSpeed = SuctionSpeed;
	DefaultWhirlwindSwirl.DefaultAngularSpeed = RotationSpeed;
	DefaultWhirlwindSwirl.DefaultLinearInSpeed = InSpeed;
}

//////////////////////////////////////////////////////////////////////////
// Input

void AFrogGameCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Whirlwind", IE_Pressed, this, &AFrogGameCharacter::Whirlwind);
	PlayerInputComponent->BindAction("Whirlwind", IE_Released, this, &AFrogGameCharacter::EndWhirlwind);

	PlayerInputComponent->BindAction("Punch", IE_Pressed, this, &AFrogGameCharacter::Punch);

	// This is here for test purposes, will activate when the powerbar is filled up.
	PlayerInputComponent->BindAction("PowerMode", IE_Pressed, this, &AFrogGameCharacter::PowerMode);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	//PlayerInputComponent->BindAction("Jump", IE_Released, this, &AFrogGameCharacter::ExecuteJump);

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
			DeactivatePowerMode();
		}
	}
	if (bScalingUp)
	{
		UpdateCharacterScale(DeltaTime);
	}
	if (bUsingWhirlwind)
	{
		FilterOccludedObjects();
		DoWhirlwind(DeltaTime);
		if (bShouldRotate)
		{
			InterpToDesiredRotation(DeltaTime);
		}
	}
}

// TODO: Choose target close to box origin AND player
void AFrogGameCharacter::FilterOccludedObjects()
{
	TArray<AActor*> TempArray{PotentialTargets};
	for (AActor* Target : TempArray)
	{
		if (Target->Implements<UEdible>())
		{
			const UEdibleComponent* SizeInfo{
				Cast<UEdibleComponent>(Target->GetComponentByClass(UEdibleComponent::StaticClass()))
			};
			if (!SizeInfo)
			{
				UE_LOG(LogTemp, Warning, TEXT("Missing reference to UEdibleComponent!"));
				continue;
			}
			const int MaxSize{SizeTier - EdibleThreshold}; // Highest size tier the player can eat
			if (SizeInfo->SizeTier > MaxSize) // Object is too big, ignore.
			{
				continue;
			}
			// this needs to happen during tick to filter out actors that shouldn't be sucked in
			// should drop the actor from the map while keeping it in the targets array for continuous checking until it leaves the area
			// probably easiest to stop checking a target once it succeeds the check
			const ETraceTypeQuery InTypeQuery{UCollisionProfile::Get()->ConvertToTraceType(ECC_Visibility)};
			const TArray<AActor*> Array;
			FHitResult Hit;
			const bool Success{
				UKismetSystemLibrary::LineTraceSingle(GetWorld(), GetActorLocation(), Target->GetActorLocation(),
				                                      InTypeQuery,
				                                      false, Array, EDrawDebugTrace::None, Hit, true)
			};
			if (!Success)
			{
				UE_LOG(LogTemp, Warning, TEXT("%s failed trace."), *Target->GetName())
			}
			// if hit actor is the same as target, no occluding actor was found
			if (Hit.GetActor() == Target)
			{
				UE_LOG(LogTemp, Warning, TEXT("Added %s to WhirlwindAffectedActors map."), *Target->GetName())
				auto& SwirlInfo = WhirlwindAffectedActors.Add(Target, DefaultWhirlwindSwirl);
				SwirlInfo.Construct();

				const FVector FrogToTarget{Target->GetActorLocation() - GetActorLocation()};
				SwirlInfo.RadianDelta = FMath::Atan2(FrogToTarget.X, FrogToTarget.Z);
				SwirlInfo.CurrentRadius = FMath::Sqrt(
					(FrogToTarget.Z * FrogToTarget.Z) + (FrogToTarget.X * FrogToTarget.X));
				UE_LOG(LogTemp, Warning, TEXT("Current Radius: %f"), SwirlInfo.CurrentRadius)
				// Max Radius needs to be a factor of distance ahead of the player
				// So if target is at max range from player, then the factor equals 1
				// if it's close, then the factor is much less 
				SwirlInfo.MaxRadius = CalcMaxRadius(Target);
				UE_LOG(LogTemp, Warning, TEXT("Max Radius: %f"), SwirlInfo.MaxRadius)

				SwirlInfo.LinearUpPosition = Target->GetActorLocation().Y - GetActorLocation().Y;

				PotentialTargets.Remove(Target);
			}
		}
	}
}

float AFrogGameCharacter::CalcMaxRadius(AActor* Actor) const
{
	const FVector2D FrogToTarget{Actor->GetActorLocation() - GetActorLocation()}; // ignore the z-value
	const float Dist{FrogToTarget.Size()};
	const float MaxDist{WhirlwindVolume->GetScaledBoxExtent().X * 2.f};
	// Clamp it to 10, or 10% of the scaled box extent in the Y axis. If distance is much less than max distance, we don't want the max radius to become impossibly tiny.
	const float Ratio{FMath::Clamp(MaxDist / Dist, 1.f, 10.f)};
	return {WhirlwindVolume->GetScaledBoxExtent().Y * 2.f / Ratio};
}

void AFrogGameCharacter::Whirlwind()
{
	UE_LOG(LogTemp, Warning, TEXT("Using whirlwind!"))
	bUsingWhirlwind = true;
}

void AFrogGameCharacter::DoWhirlwind(const float DeltaTime)
{
	// for every edible object in the volume, apply an interpolated movement towards the player, and if it gets close enough use Consume.
	// Instead of giving every object its own FSwirlInfo, we can just generate one for each new object that enters the whirlwind,
	// and place them in a map where each actor has its own FSwirlInfo for easy lookup.
	// This happens in OnWhirlwindBeginOverlap.
	for (auto It = WhirlwindAffectedActors.CreateIterator(); It; ++It)
	{
		AActor* Actor{It->Key};
		const float DistToPlayer{FVector::Dist(Actor->GetActorLocation(), GetActorLocation())};
		if (DistToPlayer <= ShrinkDistance * GetActorScale().X)
		{
			if (Actor->GetActorScale().Size() >= 0.1f)
			{
				const FVector NewScale{Actor->GetActorScale() - (FVector(0.003f) * GetActorScale().X)};
				Actor->SetActorScale3D(NewScale);
			}
		}
		if (DistToPlayer <= EatDistance * GetActorScale().X)
		{
			Consume(Actor);
			continue;
		}
		if (It->Value.CurrentRadius >= It->Value.MaxRadius)
		{
			const float RadiusRatio{It->Value.MaxRadius / It->Value.CurrentRadius};
			// If CR == MR, the result will be DefaultLinearInSpeed / 1.
			It->Value.LinearInSpeed = It->Value.DefaultLinearInSpeed / RadiusRatio;
		}
		FVector OutPosition;
		// Something to keep in mind -- FSwirlInfo's variables become somewhat misnamed.
		FrogFunctionLibrary::HorizontalSwirl(DeltaTime, It->Value, GetActorLocation(),
		                                     OutPosition);
		It->Value.MaxRadius = CalcMaxRadius(Actor);
		Actor->SetActorLocation(OutPosition);
	}
	if (GetCharacterMovement()->Velocity == FVector(0, 0, 0))
	{
		DesiredRotation = GetActorRotation();
		DesiredRotation.Yaw = FollowCamera->GetForwardVector().ToOrientationRotator().Yaw;
		bShouldRotate = true;
	}
}

void AFrogGameCharacter::InterpToDesiredRotation(const float DeltaTime)
{
	SetActorRotation(FMath::RInterpConstantTo(GetActorRotation(), DesiredRotation, DeltaTime, 400.f));
	if (DesiredRotation.Equals(GetActorRotation(), 5.f))
	{
		bShouldRotate = false;
	}
}

void AFrogGameCharacter::EndWhirlwind()
{
	bUsingWhirlwind = false;
	UE_LOG(LogTemp, Warning, TEXT("Stopped using whirlwind."))
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
	Movement->MaxAcceleration = 4096.f * GetActorScale().X;
	CurrentJump += BaseJump * (ScaleDelta * 0.2f); // 0.2f is temporary to reduce jump height increase
	Movement->JumpZVelocity = CurrentJump;
}

void AFrogGameCharacter::UpdateCameraBoom(const float ScaleDelta) const
{
	// TODO: Comment this
	CameraBoom->TargetArmLength += BaseBoomRange * ScaleDelta;
}


void AFrogGameCharacter::Consume(AActor* OtherActor)
{
	if (!OtherActor)
	{
		return;
	}
	if (OtherActor->Implements<UEdible>())
	{
		const UEdibleComponent* SizeInfo{
			Cast<UEdibleComponent>(OtherActor->GetComponentByClass(UEdibleComponent::StaticClass()))
		};
		UE_LOG(LogTemp, Warning, TEXT("%s"), *OtherActor->GetName())
		WhirlwindAffectedActors.Remove(OtherActor);
		OtherActor->Destroy();

		IncreaseScale(SizeInfo);
	}
}

void AFrogGameCharacter::IncreaseScale(const UEdibleComponent* SizeInfo)
{
	// just reset the lerp values
	bScalingUp = true;
	// We use the scaled radius value of the capsule collider to get an approximate size value for the main character.
	const float ScaledRadius{GetCapsuleComponent()->GetScaledCapsuleRadius()};
	// Compare this to the averaged bounding box size of the object being eaten and factor in the growth coefficient.
	// If SizeInfo.Size = 10 and ScaledRadius = 50 then we get a value of 10/50 = 0.2 or 20%.
	// Increase actor scale by this value. 
	float SizeDiff{SizeInfo->Size * SizeInfo->GrowthCoefficient / ScaledRadius};
	SizeDiff = FMath::Clamp(SizeDiff, 0.f, 0.1f);
	ExtraScaleBank += SizeDiff;
	//UE_LOG(LogTemp, Warning, TEXT("SizeDiff = %f"), SizeDiff)

	UpdateCurrentScore(SizeInfo->ScorePoints);
	UpdatePowerPoints(SizeInfo->PowerPoints);
}

void AFrogGameCharacter::Punch()
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
}

void AFrogGameCharacter::RemoveHandCollision()
{
	SetHandCollision(RightHandCollision, TEXT("NoCollision"));
	SetHandCollision(LeftHandCollision, TEXT("NoCollision"));
}

void AFrogGameCharacter::OnAttackOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                         UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                         const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Hit Event with %s!"), *OtherActor->GetName())

	if (OtherActor != this && OtherComp != nullptr && OtherActor->Implements<UEdible>())
	{
		RemoveHandCollision();
		UEdibleComponent* EdibleComponent{
			Cast<UEdibleComponent>(OtherActor->GetComponentByClass(UEdibleComponent::StaticClass()))
		};
		if (EdibleComponent)
		{
			TSubclassOf<UDamageType> const ValidDamageTypeClass = TSubclassOf<UDamageType>(UDamageType::StaticClass());
			const FDamageEvent DamageEvent(ValidDamageTypeClass);
			OtherActor->TakeDamage(PunchDamage, DamageEvent, GetController(), this);
		}
	}
}

void AFrogGameCharacter::OnWhirlwindBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                                 const FHitResult& SweepResult)
{
	if (OtherActor->GetComponentByClass(UEdibleComponent::StaticClass()))
	{
		PotentialTargets.Add(OtherActor);
	}
}

void AFrogGameCharacter::OnWhirlwindEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                               UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//UE_LOG(LogTemp, Warning, TEXT("Lost sight of target!"));

	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("Target stopped overlapping."))
		if (PotentialTargets.Find(OtherActor) != INDEX_NONE)
		{
			PotentialTargets.Remove(OtherActor); // Not sure if this works as intended
		}
	}
}

void AFrogGameCharacter::PowerMode()
{
	CurrentPowerPoints = MaxPowerPoints;
	bPowerMode = true;
	WhirlwindAffectedActors.Empty();
	SetPlayerModel(PowerModeSettings);
	const FAttachmentTransformRules InRule{EAttachmentRule::KeepRelative, false};

	RightHandCollision->AttachToComponent(GetMesh(), InRule, TEXT("r_wrist_j"));
	LeftHandCollision->AttachToComponent(GetMesh(), InRule, TEXT("l_wrist_j"));
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
	GetCharacterMovement()->GravityScale = CharacterSettings.GravityScale;
	GetCharacterMovement()->JumpZVelocity = CharacterSettings.BaseJumpZHeight;
	const FAttachmentTransformRules InRule{EAttachmentRule::SnapToTarget, false};
	TongueStart->AttachToComponent(GetMesh(), InRule, CharacterSettings.HeadSocket);

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


void AFrogGameCharacter::SaveGame()
{
	UFrogGameInstance* GameInstance{Cast<UFrogGameInstance>(GetGameInstance())};
	GameInstance->SaveCurrentToSlot();
}

void AFrogGameCharacter::LoadGame()
{
	Cast<UFrogGameInstance>(GetGameInstance())->LoadCheckpoint();
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
