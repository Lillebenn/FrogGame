// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "FrogGameCharacter.h"

#include "CustomDestructibleComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "Components/InputComponent.h"
#include "DestructibleObject.h"
#include "Engine.h"
#include "Engine/EngineTypes.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Edible.h"
#include "EdibleObject.h"
#include "SimpleCreature.h"
#include "SphereDrop.h"

//////////////////////////////////////////////////////////////////////////
// AFrogGameCharacter

AFrogGameCharacter::AFrogGameCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(13.f, 13.0f);

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
	WhirlwindVolume->SetCollisionProfileName(TEXT("NoCollision"));

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

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SkelMesh(
		TEXT("/Game/Models/Player/Player_Powered/sk_frog2.sk_frog2"));
	if (SkelMesh.Object)
	{
		GetMesh()->SetSkeletalMesh(SkelMesh.Object);
	}
	PunchParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Punch Particle"));
	PunchParticle->SetAutoActivate(false);
	PunchParticle->SetupAttachment(GetMesh(),TEXT("r_hand_end_j"));
	FireEyeOne = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Fire Eye 1"));
	FireEyeOne->SetAutoActivate(false);
	FireEyeOne->SetupAttachment(GetMesh(), TEXT("head_j"));
	FireEyeTwo = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Fire Eye 2"));
	FireEyeTwo->SetAutoActivate(false);
	FireEyeTwo->SetupAttachment(GetMesh(), TEXT("head_j"));
	PowerUpParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("PowerUp Particle"));
	PowerUpParticle->SetAutoActivate(false);
	PowerUpParticle->SetupAttachment(RootComponent);
	const ConstructorHelpers::FObjectFinder<UAnimMontage> AnimPunchMontage(
		TEXT("/Game/Models/Player/Player_Powered/PunchingMontage"));
	if (AnimPunchMontage.Object)
	{
		PunchMontage = AnimPunchMontage.Object;
	}
	NeutralModeBP = this->StaticClass();
}


void AFrogGameCharacter::BeginPlay()
{
	Super::BeginPlay();

	SetupSettingsCopies();

	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AFrogGameCharacter::OnOverlap);
	GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &AFrogGameCharacter::OnEndOverlap);
	WhirlwindVolume->OnComponentBeginOverlap.AddDynamic(this, &AFrogGameCharacter::OnWhirlwindBeginOverlap);
	WhirlwindVolume->OnComponentEndOverlap.AddDynamic(this, &AFrogGameCharacter::OnWhirlwindEndOverlap);
	WhirlwindRange = WhirlwindVolume->GetScaledBoxExtent().X;
	AttachedActorsSetup();
	// Setting Hud trackers to 0 at the start.
	CurrentScore = 0;
	CurrentPowerPoints = 0.f;
	FrogHealth = 1000.f;

	// Setup the default whirlwind swirl settings
	DefaultWhirlwindSwirl.DefaultLinearUpSpeed = SuctionSpeed;
	DefaultWhirlwindSwirl.DefaultAngularSpeed = RotationSpeed;
	DefaultWhirlwindSwirl.DefaultLinearInSpeed = InSpeed;
}

void AFrogGameCharacter::SetupSettingsCopies()
{
	NeutralModeSettings = DuplicateObject(this, GetOuter());
	PowerModeSettings = PowerModeBP.GetDefaultObject();
	FireEyeOne->SetTemplate(PowerModeSettings->FireEyeOne->Template);
	FireEyeTwo->SetTemplate(PowerModeSettings->FireEyeTwo->Template);
	PunchMontage = PowerModeSettings->PunchMontage;
	PunchDamage = PowerModeSettings->PunchDamage;
	PunchForwardDistance = PowerModeSettings->PunchForwardDistance;
	PunchOne = PowerModeSettings->PunchOne;
	PunchOneOffset = PowerModeSettings->PunchOneOffset;
	PunchTwo = PowerModeSettings->PunchTwo;
	PunchTwoOffset = PowerModeSettings->PunchTwoOffset;
	UpperCut = PowerModeSettings->UpperCut;
	UpperCutOffset = PowerModeSettings->UpperCutOffset;
	PunchVolumeType = PowerModeSettings->PunchVolumeType;
	PunchShake = PowerModeSettings->PunchShake;
	RightPunchVolumeYPosition = PowerModeSettings->RightPunchVolumeYPosition;
	LeftPunchVolumeYPosition = PowerModeSettings->LeftPunchVolumeYPosition;
	MaxPowerPoints = PowerModeSettings->MaxPowerPoints;
	PowerPointsDivisor = PowerModeSettings->PowerPointsDivisor;
	DrainSpeed = PowerModeSettings->DrainSpeed;
}

//////////////////////////////////////////////////////////////////////////
// Input
void AFrogGameCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &AFrogGameCharacter::Attack);
	PlayerInputComponent->BindAction("Attack", IE_Released, this, &AFrogGameCharacter::EndAttack);


	// This is here for test purposes, will activate when the powerbar is filled up.
	PlayerInputComponent->BindAction("PowerMode", IE_Pressed, this, &AFrogGameCharacter::PowerMode);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	//PlayerInputComponent->BindAction("Jump", IE_Released, this, &AFrogGameCharacter::ExecuteJump);
#if WITH_EDITOR
	PlayerInputComponent->BindAction("StopPowerMode", IE_Pressed, this, &AFrogGameCharacter::DeactivatePowerMode);
	PlayerInputComponent->BindAction("TestTrail", IE_Pressed, this, &AFrogGameCharacter::TestTrail);
	PlayerInputComponent->BindAction("ParticleTest", IE_Pressed, this, &AFrogGameCharacter::PauseMontage);
#endif
	PlayerInputComponent->BindAxis("MoveForward", this, &AFrogGameCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AFrogGameCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "TurnRate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AFrogGameCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AFrogGameCharacter::LookUpAtRate);

	PlayerInputComponent->BindAction("PauseMenu", IE_Pressed, this, &AFrogGameCharacter::OpenPauseMenu);
}


void AFrogGameCharacter::AttachedActorsSetup()
{
	if (PunchVolumeType)
	{
		PunchVolumeActor = GetWorld()->SpawnActor<AActor>(PunchVolumeType);
		PunchVolumeActor->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
		PunchVolume = PunchVolumeActor->FindComponentByClass<UBoxComponent>();
		PunchVolume->OnComponentBeginOverlap.AddDynamic(this, &AFrogGameCharacter::OnAttackOverlap);
		RegularBoxExtent = PunchVolume->GetUnscaledBoxExtent();
	}
	if (ShockwaveActor)
	{
		auto Actor{GetWorld()->SpawnActor<AActor>(ShockwaveActor)};
		Actor->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
		Actor->SetActorRelativeLocation(FVector(0.f, 0.f, -10.f));
		ShockwaveCollider = Actor->FindComponentByClass<USphereComponent>();
		ShockwaveColliderRadius = ShockwaveCollider->GetUnscaledSphereRadius();
		ShockwaveCollider->SetCollisionProfileName(TEXT("NoCollision"));
		ShockwavePFX = Actor->FindComponentByClass<UParticleSystemComponent>();
	}
}

void AFrogGameCharacter::OpenPauseMenu()
{
	// TODO: Replace this with a call to a pause menu widget, this is just for play-testing purposes so people can quit without Alt-F4
	UKismetSystemLibrary::QuitGame(GetWorld(), Cast<APlayerController>(GetController()), EQuitPreference::Quit, true);
}

void AFrogGameCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bPowerMode)
	{
		PowerDrain(DeltaTime);
		if (bPunchMove)
		{
			MoveForward(PunchForwardDistance);
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
	if ((GetVelocity().IsZero() || GetCharacterMovement()->IsFalling()) && !bTestTrail)
	{
		DisableTrail();
	}
}

void AFrogGameCharacter::FilterOccludedObjects()
{
	TArray<AActor*> TempArray{PotentialTargets};
	for (AActor* Target : TempArray)
	{
		const UEdibleComponent* Edible{Target->FindComponentByClass<UEdibleComponent>()};
		if (!Edible)
		{
			UE_LOG(LogTemp, Warning, TEXT("Missing reference to UEdibleComponent!"));
			continue;
		}
		if (!Edible->bAllowSuction)
		{
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
		const auto ImplementsEdible{
			Hit.GetActor() ? Hit.GetActor()->GetComponentByClass(UEdibleComponent::StaticClass()) : nullptr
		};
		if (Hit.GetActor() == Target || ImplementsEdible)
		{
			AEdibleObject* EdibleObject{Cast<AEdibleObject>(Target)};
			if (EdibleObject)
			{
				EdibleObject->StaticMesh->SetSimulatePhysics(false);
			}
			UEdibleComponent* EdibleComponent{Target->FindComponentByClass<UEdibleComponent>()};
			//UE_LOG(LogTemp, Warning, TEXT("Added %s to WhirlwindAffectedActors map."), *Target->GetName())
			EdibleComponent->IgnorePawnCollision();
			EdibleComponent->SetInitialTransform();
			//IEdible::Execute_PauseAI(Target, true); // TODO: Needs testing
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


void AFrogGameCharacter::Whirlwind()
{
	if (!bPowerMode)
	{
		UE_LOG(LogTemp, Warning, TEXT("Using whirlwind!"))
		bUsingWhirlwind = true;
		WhirlwindEvent(true);
		WhirlwindVolume->SetCollisionProfileName(TEXT("Whirlwind"));
		SpawnWhirlwindPfx();
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
		auto EdibleComponent{Actor->FindComponentByClass<UEdibleComponent>()};
		if (!EdibleComponent)
		{
			continue;
		}
		float& PivotDistance{EdibleComponent->PivotDistance};
		PivotDistance -= SuctionSpeed * DeltaTime;
		if (PivotDistance <= EatDistance)
		{
			Consume(Actor);
			continue;
		}
		if (PivotDistance != 0.f)
		{
			UE_LOG(LogTemp, Warning, TEXT("%f"), PivotDistance / WhirlwindRange);

			const FVector NewScale = EdibleComponent->GetInitialTransform().GetScale3D() * PivotDistance /
				WhirlwindRange;

			Actor->SetActorScale3D(NewScale);
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
	WhirlwindVolume->SetCollisionProfileName(TEXT("NoCollision"));
	DisableWhirlwindPfx();
	WhirlwindEvent(false);
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->MaxWalkSpeed = NeutralModeSettings->GetCharacterMovement()->MaxWalkSpeed;
	for (const auto Actor : WhirlwindAffectedActors)
	{
		if (auto Edible = Cast<AEdibleObject>(Actor))
		{
			Edible->StaticMesh->SetSimulatePhysics(true);
		}
		auto EdibleComp{Actor->FindComponentByClass<UEdibleComponent>()};
		if (EdibleComp->Parent)
		{
			EdibleComp->Parent->Destroy();
		}
		if (Actor->Implements<UEdible>())
		{
			IEdible::Execute_PauseAI(Actor, false);
		}

		Actor->SetActorScale3D(EdibleComp->GetInitialTransform().GetScale3D());
	}
	WhirlwindAffectedActors.Empty();

	UE_LOG(LogTemp, Warning, TEXT("Stopped using whirlwind."))
}

void AFrogGameCharacter::Attack()
{
	if (bPowerMode && !bIsInWater)
	{
		Punch();
	}
	else
	{
		Whirlwind();
	}
}

void AFrogGameCharacter::EndAttack()
{
	if (bPowerMode)
	{
		StopPunch();
	}
	else
	{
		EndWhirlwind();
	}
}

void AFrogGameCharacter::Punch()
{
	if (bPowerMode && !bIsPunching)
	{
		GetWorld()->GetTimerManager().SetTimer(PunchRepeatTimer, this, &AFrogGameCharacter::DoPunch, 0.35f,
		                                       true, 0.f);
	}
}

void AFrogGameCharacter::DoPunch()
{
	GetWorld()->GetTimerManager().SetTimer(PunchResetHandle, this, &AFrogGameCharacter::PunchReset, 0.75f,
	                                       false);
	FVector PunchVolumePosition{PunchVolume->GetUnscaledBoxExtent().X, 0.f, -10.f};
	if (GetCharacterMovement()->IsFalling())
	{
		CurrentPunch = 2;
	}
	FVector BoxExtent{RegularBoxExtent};
	switch (CurrentPunch)
	{
	case 0:
		PlayAnimMontage(PunchMontage, 1, TEXT("First Punch"));
		PunchVolumePosition.Y = RightPunchVolumeYPosition;
		break;
	case 1:
		PlayAnimMontage(PunchMontage, 1, TEXT("Second Punch"));
		PunchVolumePosition.Y = LeftPunchVolumeYPosition;
		break;
	case 2:
		PlayAnimMontage(PunchMontage, 1, TEXT("UpperCut"));
		PunchVolumePosition.Y = RightPunchVolumeYPosition;
		BoxExtent = UpperCutBoxExtent;
		PunchVolumePosition.Z = 60.f;
		break;
	default:
		break;
	}

	if (PunchVolumeActor)
	{
		PunchVolumeActor->SetActorRelativeLocation(PunchVolumePosition);
		PunchVolume->SetCollisionProfileName(TEXT("Punch"));
		PunchVolume->SetBoxExtent(BoxExtent);
	}
	CurrentPunch++;
	if (CurrentPunch > 2)
	{
		CurrentPunch = 0;
	}
	bIsPunching = true;
	bPunchMove = true;
}


void AFrogGameCharacter::PunchAnimNotify()
{
	UE_LOG(LogTemp, Warning, TEXT("Punching! Current Punch is %d"), CurrentPunch)
	const FAttachmentTransformRules InRule{EAttachmentRule::KeepRelative, false};

	switch (CurrentPunch)
	{
	case 0:
		if (UpperCut)
		{
			PunchParticle->SetTemplate(UpperCut);
		}
		PunchParticle->AttachToComponent(GetMesh(), InRule,
		                                 TEXT("r_hand_end_j"));
		PunchParticle->SetRelativeLocation(UpperCutOffset);
		break;
	case 1:
		if (PunchOne)
		{
			PunchParticle->SetTemplate(PunchOne);
		}
		PunchParticle->AttachToComponent(GetMesh(), InRule,
		                                 TEXT("r_hand_end_j"));
		PunchParticle->SetRelativeLocation(PunchOneOffset);
		break;
	case 2:
		if (PunchTwo)
		{
			PunchParticle->SetTemplate(PunchTwo);
		}
		PunchParticle->AttachToComponent(GetMesh(), InRule,
		                                 TEXT("l_hand_end_j"));
		PunchParticle->SetRelativeLocation(PunchTwoOffset);
		break;
	default:
		break;
	}
	if (HitActors.Num() > 0)
	{
		PunchParticle->Activate(true);
	}
	if (bShouldPauseMontage)
	{
		PunchParticle->Activate(true);
		GetMesh()->GetAnimInstance()->Montage_Pause(GetMesh()->GetAnimInstance()->GetCurrentActiveMontage());
	}
	ApplyDamage();
	bIsPunching = false;
	bPunchMove = false;
}

void AFrogGameCharacter::PunchReset()
{
	CurrentPunch = 0;
}

void AFrogGameCharacter::Consume_Impl(AActor* OtherActor)
{
	UEdibleComponent* Edible{OtherActor->FindComponentByClass<UEdibleComponent>()};
	const bool bCanConsume{Edible ? !Edible->bConsumed : false};
	if (bCanConsume)
	{
		Edible->bConsumed = true;
		UpdateCurrentScore(Edible->ScorePoints);
		if (bPowerMode)
		{
			UpdatePowerPoints(Edible->PowerPoints / PowerPointsDivisor);
		}
		else
		{
			UpdatePowerPoints(Edible->PowerPoints);
		}
		UE_LOG(LogTemp, Warning, TEXT("Destroying %s"), *OtherActor->GetName())
		OtherActor->SetLifeSpan(0.001f);
	}
}

void AFrogGameCharacter::Consume(AActor* OtherActor)
{
	if (OtherActor)
	{
		Consume_Impl(OtherActor);
		WhirlwindAffectedActors.Remove(OtherActor);
	}
}

void AFrogGameCharacter::Consume(ASphereDrop* Sphere)
{
	Consume_Impl(Sphere);
}

void AFrogGameCharacter::ApplyDamage()
{
	for (auto Actor : HitActors)
	{
		Actor->TakeDamage(PunchDamage, FDamageEvent(), GetController(), this);
	}
	if (HitActors.Num() > 0 && PunchShake)
	{
		GetWorld()->GetFirstPlayerController()->PlayerCameraManager->PlayCameraShake(PunchShake, 3);
	}
	HitActors.Empty();
	PunchVolume->SetCollisionProfileName(TEXT("NoCollision"));
}

void AFrogGameCharacter::StopPunch()
{
	GetWorld()->GetTimerManager().ClearTimer(PunchRepeatTimer);
	if (PunchVolume)
	{
		PunchVolume->SetCollisionProfileName(TEXT("NoCollision"));
	}
}


void AFrogGameCharacter::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                   const FHitResult& SweepResult)
{
	if (OtherActor != nullptr && OtherActor != this && OtherComp != nullptr)
	{
		if (OtherActor->IsA(SmallDestructible))
		{
			// TODO: Maybe do a different function if we don't want shit to fly around
			OtherActor->TakeDamage(PunchDamage, FDamageEvent(), GetController(), this);
		}
		else if (OtherComp->ComponentHasTag(TEXT("Water")))
		{
			WaterFloor = OtherActor->FindComponentByClass<UBoxComponent>();
			Swim(true);
		}
	}
}

void AFrogGameCharacter::OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                      UPrimitiveComponent* OtherComp,
                                      int32 OtherBodyIndex)
{
	if (OtherActor != nullptr && OtherActor != this && OtherComp != nullptr)
	{
		if (OtherComp->ComponentHasTag(TEXT("Water")))
		{
			Swim(false);
		}
		WaterFloor = nullptr;
	}
}

void AFrogGameCharacter::Swim(const bool bActivate)
{
	bIsInWater = bActivate;
	DisableTrail();
	WalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
	if (bActivate)
	{
		switch (CurrentMode)
		{
		case ECharacterMode::Neutral:
			GetCharacterMovement()->MaxWalkSpeed = NeutralModeSettings->SwimSpeed;
			break;
		case ECharacterMode::Power:
			GetCharacterMovement()->MaxWalkSpeed = PowerModeSettings->SwimSpeed;
			break;
		default:
			break;
		}
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	}
	if (bPowerMode)
	{
		if (WaterFloor)
		{
			if (bActivate)
			{
				WaterFloor->SetRelativeLocation(FVector(0, 0, -150.f));
			}
			else
			{
				WaterFloor->SetRelativeLocation(FVector(0, 0, -200.f));
			}
		}
		EndAttack();
	}
}

void AFrogGameCharacter::OnAttackOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                         UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                         const FHitResult& SweepResult)
{
	if (OtherActor != nullptr && OtherActor != this && OtherComp != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Hit Event with %s!"), *OtherActor->GetName())

		if (!HitActors.Contains(OtherActor))
		{
			HitActors.Add(OtherActor);
		}
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
		//UE_LOG(LogTemp, Warning, TEXT("Target stopped overlapping."))
		if (PotentialTargets.Find(OtherActor) != INDEX_NONE)
		{
			PotentialTargets.Remove(OtherActor); // Not sure if this works as intended
		}
	}
}

void AFrogGameCharacter::PowerMode()
{
	if (CurrentPowerPoints >= MaxPowerPoints / 10.f && !bPowerMode)
	{
		bPowerMode = true;
		if (bUsingWhirlwind)
		{
			EndWhirlwind();
		}
		if (WaterFloor && bIsInWater)
		{
			WaterFloor->SetRelativeLocation(FVector(0.f, 0.f, -150.f));
		}
		CurrentMode = ECharacterMode::Power;
		SetPlayerModel(PowerModeSettings);
		PowerUpParticle->Activate(true);
		FireEyeOne->Activate();
		FireEyeTwo->Activate();
	}
}

void AFrogGameCharacter::DeactivatePowerMode()
{
	bPowerMode = false;
	EndAttack();
	CurrentMode = ECharacterMode::Neutral;
	SetPlayerModel(NeutralModeSettings);
	FireEyeOne->Deactivate();
	FireEyeTwo->Deactivate();
	DisableWhirlwindPfx();
	HitActors.Empty();
	if (WaterFloor && bIsInWater)
	{
		WaterFloor->SetRelativeLocation(FVector(0.f, 0.f, -200.f));
	}
	if (PunchVolume)
	{
		PunchVolume->SetCollisionProfileName(TEXT("NoCollision"));
	}
}

void AFrogGameCharacter::SetPlayerModel(AFrogGameCharacter* CharacterSettings)
{
	GetMesh()->SetAnimInstanceClass(CharacterSettings->GetMesh()->GetAnimClass());
	GetMesh()->SetSkeletalMesh(CharacterSettings->GetMesh()->SkeletalMesh);
	UCapsuleComponent* Capsule{CharacterSettings->GetCapsuleComponent()};
	const float ScaledHalfHeight{Capsule->GetScaledCapsuleHalfHeight()};
	GetCapsuleComponent()->SetCapsuleSize(Capsule->GetUnscaledCapsuleRadius(),
	                                      Capsule->GetUnscaledCapsuleHalfHeight());
	GetMesh()->SetRelativeLocation(CharacterSettings->GetMesh()->GetRelativeLocation());
	SetActorScale3D(FVector(Capsule->GetRelativeScale3D()));
	if (CurrentMode == ECharacterMode::Neutral)
	{
		const float ScaledCapsuleHalfHeightDiff{
			ScaledHalfHeight
			- GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight()
		};
		UE_LOG(LogTemp, Error, TEXT("%f"), ScaledCapsuleHalfHeightDiff)
		SetActorLocation(GetActorLocation() + FVector(0.f, 0.f, -ScaledCapsuleHalfHeightDiff));
	}
	UCharacterMovementComponent* MovementComponent{CharacterSettings->GetCharacterMovement()};
	GetCharacterMovement()->MaxWalkSpeed = MovementComponent->MaxWalkSpeed;
	GetCharacterMovement()->GravityScale = MovementComponent->GravityScale;
	GetCharacterMovement()->JumpZVelocity = MovementComponent->JumpZVelocity;
	SmokeTrailOffset.Z = CharacterSettings->SmokeTrailOffset.Z;
	SmokeTrailScale = CharacterSettings->SmokeTrailScale;
	WaterBreakOffset = CharacterSettings->WaterBreakOffset;
	WaterBreakScale = CharacterSettings->WaterBreakScale;
	FireEyeOne->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform,
	                              CharacterSettings->FireEyeOne->GetAttachSocketName());
	FireEyeTwo->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform,
	                              CharacterSettings->FireEyeTwo->GetAttachSocketName());
}


void AFrogGameCharacter::UpdatePowerPoints(float Points)
{
	CurrentPowerPoints = CurrentPowerPoints + Points;
	if (CurrentPowerPoints >= MaxPowerPoints)
	{
		CurrentPowerPoints = MaxPowerPoints;
		PowerMode();
	}
}

void AFrogGameCharacter::PowerDrain(float DeltaTime)
{
	const float DrainPoints = (DeltaTime * DrainSpeed);
	UpdatePowerPoints(DrainPoints);
	if (CurrentPowerPoints <= 0.f)
	{
		DeactivatePowerMode();
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

void AFrogGameCharacter::SpawnWhirlwindPfx()
{
	if (BPWhirlwindPFX && !WhirlwindPFX)
	{
		WhirlwindPFX = GetWorld()->SpawnActor<AActor>(BPWhirlwindPFX);
		const FAttachmentTransformRules InRule{EAttachmentRule::SnapToTarget, false};
		WhirlwindPFX->AttachToComponent(GetMesh(), InRule, TEXT("joint6"));

		WhirlwindPFX->SetActorRelativeRotation(FRotator(-90.f, -45.f, 0.f));
		WhirlwindPFX->SetActorRelativeLocation(FVector(225.f, -75.f, 0.f));

		//WhirlwindPFX->SetActorRelativeScale3D(FVector(0.1f));
	}
}

void AFrogGameCharacter::DisableWhirlwindPfx()
{
	if (WhirlwindPFX)
	{
		WhirlwindPFX->Destroy();
		WhirlwindPFX = nullptr;
	}
}

void AFrogGameCharacter::Jump()
{
	InitialZValue = GetActorLocation().Z;
	bFirstJump = true;
	DisableTrail();
	Super::Jump();
	ShockwaveCollider->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	ShockwaveCollider->SetSphereRadius(ShockwaveColliderRadius);
}

void AFrogGameCharacter::IncreaseGravity() const
{
	const float CurrentGravityScale{GetCharacterMovement()->GravityScale};
	GetCharacterMovement()->GravityScale = CurrentGravityScale * 3.f;
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
		if (bIsInWater)
		{
			SpawnTrail(WaterBreakChild, WaterBreakOffset, WaterBreakScale, WaterBreakRot);
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
		if (bIsInWater)
		{
			SpawnTrail(WaterBreakChild, WaterBreakOffset, WaterBreakScale, WaterBreakRot);
		}
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}


void AFrogGameCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	switch (CurrentMode)
	{
	case ECharacterMode::Neutral:
		GetCharacterMovement()->GravityScale = NeutralModeSettings->GetCharacterMovement()->GravityScale;
		break;
	case ECharacterMode::Power:
		GetCharacterMovement()->GravityScale = PowerModeSettings->GetCharacterMovement()->GravityScale;
	}
	// use this event to add shockwave etc
	if (bFirstJump)
	{
		if (bIsInWater)
		{
			if (WaterShockwave)
			{
				ShockwavePFX->SetTemplate(WaterShockwave);
				ShockwavePFX->SetRelativeScale3D(FVector(WaterShockwaveScale));
			}
		}
		else
		{
			if (LandShockwave)
			{
				ShockwavePFX->SetTemplate(LandShockwave);
				ShockwavePFX->SetRelativeScale3D(FVector(LandShockwaveScale));
			}
		}
		ShockwavePFX->Activate(true);
		const float ZDiff{GetActorLocation().Z - InitialZValue};
		if (ZDiff < 0.f)
		{
			const float AdditionalSize{FMath::Abs(ZDiff) * 0.05f};
			const float NewSize{ShockwaveCollider->GetUnscaledSphereRadius() + AdditionalSize};
			ShockwaveCollider->SetSphereRadius(NewSize);
			if (ShockwaveShake)
			{
				GetWorld()->GetFirstPlayerController()->PlayerCameraManager->PlayCameraShake(ShockwaveShake, 2);
			}
		}
	}
	TArray<AActor*> OverlappingActors;
	ShockwaveCollider->GetOverlappingActors(OverlappingActors);
	for (auto Actor : OverlappingActors)
	{
		if (Actor->GetComponentByClass(UCustomDestructibleComponent::StaticClass()))
		{
			Actor->TakeDamage(PunchDamage, FDamageEvent(), GetController(), this);
		}
	}
	ShockwaveCollider->SetCollisionProfileName(TEXT("NoCollision"));
}

void AFrogGameCharacter::TestTrail()
{
	if (bTestTrail)
	{
		bTestTrail = false;
		DisableTrail();
	}
	else
	{
		bTestTrail = true;

		SpawnTrail(WaterBreakChild, WaterBreakOffset, WaterBreakScale, WaterBreakRot);
	}
}

void AFrogGameCharacter::PauseMontage()
{
	bShouldPauseMontage = !bShouldPauseMontage;
	if (!bShouldPauseMontage)
	{
		GetMesh()->GetAnimInstance()->Montage_Resume(GetMesh()->GetAnimInstance()->GetCurrentActiveMontage());
	}
}

void AFrogGameCharacter::SpawnTrail(const TSubclassOf<AActor> TrailType, const FVector Offset, const FVector Scale,
                                    const FRotator Rotation)
{
	if (!CurrentTrail)
	{
		CurrentTrail = GetWorld()->SpawnActor<AActor>(TrailType);
		const FAttachmentTransformRules InRule{EAttachmentRule::SnapToTarget, false};
		CurrentTrail->AttachToActor(this, InRule);
		CurrentTrail->SetActorScale3D(Scale);
		CurrentTrail->SetActorRelativeLocation(Offset);
		CurrentTrail->SetActorRelativeRotation(Rotation);
	}
}

void AFrogGameCharacter::DisableTrail()
{
	if (CurrentTrail)
	{
		CurrentTrail->SetLifeSpan(1.f);
		CurrentTrail->GetComponentByClass(UParticleSystemComponent::StaticClass())->Deactivate();
		CurrentTrail->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		CurrentTrail = nullptr;
	}
}
