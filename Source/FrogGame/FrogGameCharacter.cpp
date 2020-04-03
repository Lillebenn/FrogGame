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

	PowerModeSettings.CapsuleSize = FVector2D(10.f, 24.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	auto Movement{GetCharacterMovement()};
	Movement->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	Movement->RotationRate = FRotator(0.0f, 1080.0f, 0.0f); // ...at this rotation rate
	Movement->JumpZVelocity = 3000.f;
	Movement->MaxWalkSpeed = 1600.f;
	Movement->GravityScale = 6.f;
	Movement->AirControl = 0.2f;
	Movement->MaxAcceleration = 18000.f;

	WhirlwindVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxTrace"));
	WhirlwindVolume->SetupAttachment(RootComponent);
	WhirlwindVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	WhirlwindVolume->SetCollisionProfileName(TEXT("Whirlwind"));

	WhirlwindParticles = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Whirlwind Particle System"));
	WhirlwindParticles->SetupAttachment(RootComponent);
	WhirlwindParticles->SetVisibility(false);
	WhirlwindPivot = CreateDefaultSubobject<UChildActorComponent>(TEXT("Whirlwind Pivot"));
	WhirlwindPivot->bEditableWhenInherited = true;
	WhirlwindPivot->SetupAttachment(WhirlwindParticles);
	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 1400.f;
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	// Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	GetArrowComponent()->bEditableWhenInherited = true;

	// Power mode punch volumes need to be changed once we get the correct mesh
	// Creates a collision sphere and attaches it to the characters right hand.
	RightHandCollision = CreateDefaultSubobject<USphereComponent>(TEXT("RightHandCollision"));
	RightHandCollision->CanCharacterStepUpOn = ECB_No;
	RightHandCollision->InitSphereRadius(60.f);
	RightHandCollision->SetRelativeLocation(FVector(0.f, 9.f, 0.f));
	// Creates a collision sphere and attaches it to the characters left hand.
	LeftHandCollision = CreateDefaultSubobject<USphereComponent>(TEXT("LeftHandCollision"));
	LeftHandCollision->CanCharacterStepUpOn = ECB_No;
	LeftHandCollision->InitSphereRadius(60.f);
	LeftHandCollision->SetRelativeLocation(FVector(0.f, -9.f, 0.f));
	RemoveHandCollision();
	PunchParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Punch Particle"));
	PunchParticle->SetAutoActivate(false);
	PunchParticle->SetupAttachment(RightHandCollision);
	// PowerModeSettings defaults
	PowerModeSettings.MaxWalkSpeed = 1600.f;
	PowerModeSettings.JumpZHeight = 2000.f;
	PowerModeSettings.MeshScale = 0.3f;
	const ConstructorHelpers::FObjectFinder<USkeletalMesh> SkeletalMesh(
		TEXT("/Game/Models/Player/Player_Powered/sk_frog2.sk_frog2"));
	if (SkeletalMesh.Object)
	{
		PowerModeSettings.Mesh = SkeletalMesh.Object;
	}
	const ConstructorHelpers::FObjectFinder<UAnimMontage> AnimPunchMontage(
		TEXT("/Game/Models/Player/Player_Powered/PunchingMontage"));
	if (AnimPunchMontage.Object)
	{
		PunchMontage = AnimPunchMontage.Object;
	}
	//const ConstructorHelpers::FClassFinder<UAnimInstance> AnimInstance(TEXT("/Game/Models/Player/Player_Powered/animBP_PoweredFrog"));
	//if(AnimInstance.Class)
	//{
	//	PowerModeSettings.AnimBP = AnimInstance.Class;
	//}
}


void AFrogGameCharacter::BeginPlay()
{
	Super::BeginPlay();


	WhirlwindVolume->OnComponentBeginOverlap.AddDynamic(this, &AFrogGameCharacter::OnWhirlwindBeginOverlap);
	WhirlwindVolume->OnComponentEndOverlap.AddDynamic(this, &AFrogGameCharacter::OnWhirlwindEndOverlap);
	LeftHandCollision->OnComponentBeginOverlap.AddDynamic(this, &AFrogGameCharacter::OnAttackOverlap);
	RightHandCollision->OnComponentBeginOverlap.AddDynamic(this, &AFrogGameCharacter::OnAttackOverlap);

	// Setting Hud trackers to 0 at the start.
	CurrentScore = 0.f;
	CurrentPowerPoints = 0.f;

	NeutralModeSettings.Mesh = GetMesh()->SkeletalMesh;
	NeutralModeSettings.AnimBP = GetMesh()->GetAnimInstance()->GetClass();
	FVector2D Capsule;
	GetCapsuleComponent()->GetUnscaledCapsuleSize(Capsule.X, Capsule.Y);
	NeutralModeSettings.CapsuleSize = Capsule;
	NeutralModeSettings.MeshScale = GetMesh()->GetRelativeScale3D().X;
	NeutralModeSettings.BoomRange = CameraBoom->TargetArmLength;
	NeutralModeSettings.MaxWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
	NeutralModeSettings.JumpZHeight = GetCharacterMovement()->JumpZVelocity;
	NeutralModeSettings.GravityScale = GetCharacterMovement()->GravityScale;

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
	else if (bUsingWhirlwind)
	{
		FilterOccludedObjects();
		DoWhirlwind(DeltaTime);
		FRotator Orientation{FollowCamera->GetForwardVector().ToOrientationRotator()};
		Orientation.Pitch = 0.f;

		SetActorRotation(Orientation);
	}
	// Spawn a child actor that is JUST the particle emitter and set the lifetime of that to 1s when movement is stopped
	if (GetVelocity().IsZero() && CurrentSmokeTrail)
	{
		CurrentSmokeTrail->SetLifeSpan(1.f);
		CurrentSmokeTrail->GetComponentByClass(UParticleSystemComponent::StaticClass())->Deactivate();
		CurrentSmokeTrail->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		CurrentSmokeTrail = nullptr;
	}
	else if (!CurrentSmokeTrail && !GetVelocity().IsZero())
	{
		CurrentSmokeTrail = GetWorld()->SpawnActor<AActor>(SmokeTrailChild);
		const FAttachmentTransformRules InRule{EAttachmentRule::SnapToTarget, false};
		CurrentSmokeTrail->AttachToActor(this, InRule);
		CurrentSmokeTrail->SetActorScale3D(FVector(0.5f));
		CurrentSmokeTrail->SetActorRelativeLocation(FVector(0, 0, -15.f));
	}
}


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
			// this needs to happen during tick to filter out actors that shouldn't be sucked in
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
				AEdibleObject* Edible{Cast<AEdibleObject>(Target)};
				if (Edible)
				{
					Edible->StaticMesh->SetSimulatePhysics(false);
				}
				UEdibleComponent* EdibleComponent{
					Cast<UEdibleComponent>(Target->GetComponentByClass(UEdibleComponent::StaticClass()))
				};
				UE_LOG(LogTemp, Warning, TEXT("Added %s to WhirlwindAffectedActors map."), *Target->GetName())
				IEdible::Execute_IgnorePawnCollision(Target);
				IEdible::Execute_PauseAI(Target, true); // TODO: Needs testing
				// Parent the object to an invisible object at the pivot point
				// Rotate the entire actor in the X axis
				FVector2D FrogXY{Target->GetActorLocation() - GetActorLocation()};
				EdibleComponent->PivotDistance = FrogXY.Size();
				FVector Pivot{GetActorLocation() + GetActorForwardVector() * EdibleComponent->PivotDistance};

				EdibleComponent->Parent = GetWorld()->SpawnActor<AActor>(PivotActor, Pivot, FRotator());
				if (EdibleComponent->Parent)
				{
					FAttachmentTransformRules InRule{EAttachmentRule::KeepWorld, false};
					Target->AttachToActor(EdibleComponent->Parent, InRule);
				}

				WhirlwindAffectedActors.Add(Target);
				PotentialTargets.Remove(Target);
			}
		}
	}
}


void AFrogGameCharacter::Whirlwind()
{
	if (!bPowerMode)
	{
		UE_LOG(LogTemp, Warning, TEXT("Using whirlwind!"))
		bUsingWhirlwind = true;
		WhirlwindParticles->SetVisibility(true);
		GetCharacterMovement()->bOrientRotationToMovement = false;
		GetCharacterMovement()->MaxWalkSpeed = WhirlwindWalkSpeed;
	}
}

void AFrogGameCharacter::DoWhirlwind(float DeltaTime)
{
	// for every edible object in the volume, apply an interpolated movement towards the player, and if it gets close enough use Consume.
	// Instead of giving every object its own FSwirlInfo, we can just generate one for each new object that enters the whirlwind,
	// and place them in a map where each actor has its own FSwirlInfo for easy lookup.
	// This happens in OnWhirlwindBeginOverlap.
	TArray<AActor*> TempArray{WhirlwindAffectedActors};
	for (auto Actor : TempArray)
	{
		auto EdibleComponent{Cast<UEdibleComponent>(Actor->GetComponentByClass(UEdibleComponent::StaticClass()))};

		float& PivotDistance{EdibleComponent->PivotDistance};
		PivotDistance -= SuctionSpeed * DeltaTime;
		if (PivotDistance <= ShrinkDistance)
		{
			if (Actor->GetActorScale().Size() >= 0.1f)
			{
				const FVector NewScale{
					Actor->GetActorScale() - ISaveable::Execute_GetStartTransform(Actor).GetScale3D() * ShrinkSpeed
				};
				Actor->SetActorScale3D(NewScale);
			}
		}
		if (PivotDistance <= EatDistance)
		{
			Consume(Actor);
			continue;
		}

		AActor* Parent{EdibleComponent->Parent};
		if (!Parent)
		{
			UE_LOG(LogTemp, Warning, TEXT("Pivot not found!"))
			continue;
		}
		FVector ParentToActor{Actor->GetActorLocation() - Parent->GetActorLocation()};
		// Rotate the actor around the pivot
		FVector NewRelativePosition{ParentToActor.RotateAngleAxis(RotationSpeed, GetActorForwardVector())};
		if (FVector::Dist(Actor->GetActorLocation(), Parent->GetActorLocation()) > MinRadius)
		{
			// Move the actor slightly towards the pivot point
			NewRelativePosition -= ParentToActor.GetSafeNormal() * InSpeed * DeltaTime;
		}
		Actor->SetActorRelativeLocation(NewRelativePosition);

		// Move the pivot actor slightly towards the player
		FVector NewPosition{GetActorLocation() + GetActorForwardVector() * PivotDistance};
		Parent->SetActorLocation(NewPosition);
	}
}


void AFrogGameCharacter::EndWhirlwind()
{
	bUsingWhirlwind = false;
	WhirlwindParticles->SetVisibility(false);
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->MaxWalkSpeed = NeutralModeSettings.MaxWalkSpeed;
	for (const auto Actor : WhirlwindAffectedActors)
	{
		if (auto Edible = Cast<AEdibleObject>(Actor))
		{
			Edible->StaticMesh->SetSimulatePhysics(true);
		}
		auto EdibleComp{Cast<UEdibleComponent>(Actor->GetComponentByClass(UEdibleComponent::StaticClass()))};
		if (EdibleComp->Parent)
		{
			EdibleComp->Parent->Destroy();
		}
		IEdible::Execute_PauseAI(Actor, false);
	}
	WhirlwindAffectedActors.Empty();

	UE_LOG(LogTemp, Warning, TEXT("Stopped using whirlwind."))
}

void AFrogGameCharacter::Consume(AActor* OtherActor)
{
	if (!OtherActor)
	{
		return;
	}
	const UEdibleComponent* SizeInfo{
		Cast<UEdibleComponent>(OtherActor->GetComponentByClass(UEdibleComponent::StaticClass()))
	};
	if (SizeInfo)
	{
		UpdateCurrentScore(SizeInfo->ScorePoints);
		UpdatePowerPoints(SizeInfo->PowerPoints);
		UE_LOG(LogTemp, Warning, TEXT("Destroying %s"), *OtherActor->GetName())
		WhirlwindAffectedActors.Remove(OtherActor);
		OtherActor->Destroy();
	}
}

void AFrogGameCharacter::PunchAnimNotify() const
{
	UE_LOG(LogTemp, Warning, TEXT("Punching! Current Punch is %d"), CurrentPunch)
	switch (CurrentPunch)
	{
	case 0:
		if (UpperCut)
		{
			PunchParticle->SetTemplate(UpperCut);
		}
		PunchParticle->AttachToComponent(RightHandCollision, FAttachmentTransformRules::KeepRelativeTransform);
		PunchParticle->SetRelativeLocation(UpperCutOffset);
		break;
	case 1:
		if (PunchOne)
		{
			PunchParticle->SetTemplate(PunchOne);
		}
		PunchParticle->AttachToComponent(RightHandCollision, FAttachmentTransformRules::KeepRelativeTransform);
		PunchParticle->SetRelativeLocation(PunchOneOffset);
		break;
	case 2:
		if (PunchTwo)
		{
			PunchParticle->SetTemplate(PunchTwo);
		}
		PunchParticle->AttachToComponent(LeftHandCollision, FAttachmentTransformRules::KeepRelativeTransform);
		PunchParticle->SetRelativeLocation(PunchTwoOffset);
		break;
	default:
		break;
	}
	PunchParticle->Activate(true);
}

void AFrogGameCharacter::PunchReset()
{
	CurrentPunch = 0;
}

void AFrogGameCharacter::Punch()
{
	if (bPowerMode && !bIsPunching)
	{
		// For later, when we have a proper punch animation, turn on collision only when the punch of the player is at the end point
		// Also need to make sure the animation cancels early if it hits something, or turns off collision once it does, so it doesn't keep adding collision events.
		GetWorld()->GetTimerManager().SetTimer(PunchEndHandle, this, &AFrogGameCharacter::OnPunchEnd, 0.25f,
		                                       false);
		GetWorld()->GetTimerManager().SetTimer(PunchResetHandle, this, &AFrogGameCharacter::PunchReset, 0.75f,
		                                       false);
		RightHandCollision->SetCollisionProfileName(TEXT("Punch"));
		LeftHandCollision->SetCollisionProfileName(TEXT("Punch"));
		switch (CurrentPunch)
		{
		case 0:
			PlayAnimMontage(PunchMontage, 1, TEXT("First Punch"));
			break;
		case 1:
			PlayAnimMontage(PunchMontage, 1, TEXT("Second Punch"));
			break;
		case 2:
			PlayAnimMontage(PunchMontage, 1, TEXT("UpperCut"));
			break;
		default:
			break;
		}
		CurrentPunch++;
		if (CurrentPunch > 2)
		{
			CurrentPunch = 0;
		}
		bIsPunching = true;
		// Add execution here
	}
}

void AFrogGameCharacter::OnPunchEnd()
{
	RemoveHandCollision();
	bIsPunching = false;
}

void AFrogGameCharacter::RemoveHandCollision() const
{
	RightHandCollision->SetCollisionProfileName(TEXT("NoCollision"));
	LeftHandCollision->SetCollisionProfileName(TEXT("NoCollision"));
}

void AFrogGameCharacter::OnAttackOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                         UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                         const FHitResult& SweepResult)
{
	if (OtherActor != nullptr && OtherActor != this && OtherComp != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Hit Event with %s!"), *OtherActor->GetName())

		RemoveHandCollision();
		OtherActor->TakeDamage(PunchDamage, FDamageEvent(), GetController(), this);
	}
}

void AFrogGameCharacter::OnWhirlwindBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                                                 bool bFromSweep,
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
	EndWhirlwind();
	SetPlayerModel(PowerModeSettings);
	const FAttachmentTransformRules InRule{EAttachmentRule::KeepRelative, false};

	RightHandCollision->AttachToComponent(GetMesh(), InRule, TEXT("r_wrist_j"));
	LeftHandCollision->AttachToComponent(GetMesh(), InRule, TEXT("l_wrist_j"));
	// Change from frog mesh and rig to power-frog mesh & rig here.
}

void AFrogGameCharacter::SetPlayerModel(const FCharacterSettings& CharacterSettings) const
{
	GetMesh()->SetAnimInstanceClass(CharacterSettings.AnimBP);
	GetMesh()->SetSkeletalMesh(CharacterSettings.Mesh);

	GetCapsuleComponent()->SetCapsuleSize(CharacterSettings.CapsuleSize.X, CharacterSettings.CapsuleSize.Y);
	const FVector Offset{0, 0, -CharacterSettings.CapsuleSize.Y};
	GetMesh()->SetRelativeLocation(Offset);
	GetMesh()->SetRelativeScale3D(FVector(CharacterSettings.MeshScale));
	GetCharacterMovement()->MaxWalkSpeed = CharacterSettings.MaxWalkSpeed;
	GetCharacterMovement()->GravityScale = CharacterSettings.GravityScale;
	GetCharacterMovement()->JumpZVelocity = CharacterSettings.JumpZHeight;
}

void AFrogGameCharacter::DeactivatePowerMode()
{
	bPowerMode = false;
	SetPlayerModel(NeutralModeSettings);
	RemoveHandCollision();
}

void AFrogGameCharacter::UpdatePowerPoints(float Points)
{
	CurrentPowerPoints = CurrentPowerPoints + Points;
	if (CurrentPowerPoints >= MaxPowerPoints)
	{
		CurrentPowerPoints = MaxPowerPoints;
		// TODO: Don't force the player into power mode
		PowerMode();
	}
}

void AFrogGameCharacter::PowerDrain(float DeltaTime)
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
		if (Value < 0.f && bUsingWhirlwind)
		{
			Value = -0.5f;
		}
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
		if (Value < 0.f && bUsingWhirlwind)
		{
			Value = -0.5f;
		}
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}


void AFrogGameCharacter::Landed(const FHitResult& Hit)
{
	// use this event to add shockwave etc
}
