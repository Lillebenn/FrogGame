// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Components/SphereComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TimerManager.h"
#include "FFrogLibrary.h"
#include "SphereDrop.h"

#include "FrogGameCharacter.generated.h"
USTRUCT(BlueprintType)
struct FCharacterSettings
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class USkeletalMesh* Mesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<class UAnimInstance> AnimBP;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FVector2D CapsuleSize{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MeshScale{0.3f};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float BoomRange{1400.f};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MaxWalkSpeed{1600.f};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float JumpZHeight{2000.f};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float GravityScale{3.f};
	// Add other variables here based on what we change between modes.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float SmokeTrailZPos{-25.f};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float SmokeTrailScale{0.25f};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float WaterTrailZPos{0.f};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float WaterTrailScale{1.f};
};

enum class ECharacterMode
{
	Neutral,
	Power
};

UCLASS(config=Game, Abstract)
class AFrogGameCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;
	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* WhirlwindVolume;

	UPROPERTY (EditDefaultsOnly, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* PunchMontage;
	UPROPERTY (VisibleAnywhere, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	class UParticleSystemComponent* PunchParticle;


	UPROPERTY()
	FCharacterSettings NeutralModeSettings;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character | PowerMode", meta = (AllowPrivateAccess =
		"true"))
	FCharacterSettings PowerModeSettings;

	FTimerHandle PunchRepeatTimer;
	FTimerHandle PunchResetHandle;


public:
	AFrogGameCharacter();


	/** Accessor Function for Current Score */
	UFUNCTION(BlueprintCallable, Category = "Character | Score")
	int GetCurrentScore() const
	{
		return CurrentScore;
	}

	/** Updates the players score 
	* @Param Score This is the amount to increase the players score by. This should only be positive!
	*/
	UFUNCTION(BlueprintCallable, Category = "Character | Score")
	void UpdateCurrentScore(const int Score)
	{
		CurrentScore = CurrentScore + Score;
		UE_LOG(LogTemp, Warning, TEXT("Score gained: %d"), Score);
		UE_LOG(LogTemp, Warning, TEXT("Current score: %d"), CurrentScore);
	}

	/** Accessor Function for Current PowerPoints */
	UFUNCTION(BlueprintCallable, Category = "Character | PowerMode")
	float GetCurrentPowerPoints() const
	{
		return CurrentPowerPoints;
	}

	/** Accessor Function for Current Frog Mode */
	UFUNCTION(BlueprintCallable, Category = "Character | PowerMode")
	bool GetCurrentFrogState() const
	{
		return bPowerMode;
	}

	/** Accessor Function for Current Health */
	UFUNCTION(BlueprintCallable, Category = "Character | Health")
	float GetCurrentFrogHeatlh() const
	{
		return FrogHealth;
	}

	/**
	* @Param Points This is the amount to increase the players powerpoints by. This should only be positive on objects!
	*/
	UFUNCTION(BlueprintCallable, Category = "Character | PowerMode")
	void UpdatePowerPoints(float Points);

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	float CurrentTargetScore{0.f};

	ECharacterMode CurrentMode{ECharacterMode::Neutral};

	void Consume(AActor* OtherActor);
	void Consume(ASphereDrop* Sphere);
	UPROPERTY (EditDefaultsOnly, BlueprintReadWrite, Category = "Character | Particles")
	class UParticleSystem* PunchOne;
	UPROPERTY (EditDefaultsOnly, BlueprintReadWrite, Category = "Character | Particles")
	class UParticleSystem* PunchTwo;
	UPROPERTY (EditDefaultsOnly, BlueprintReadWrite, Category = "Character | Particles")
	class UParticleSystem* UpperCut;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | PowerMode")
	FVector PunchOneOffset{25, 50, 0};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | PowerMode")
	FVector PunchTwoOffset{-50, 0, 25};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | PowerMode")
	FVector UpperCutOffset{80, 0, 0};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | PowerMode")
	bool bPowerMode{false};
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character | PowerMode")
	bool bIsPunching{false};
	UPROPERTY(EditAnywhere, Category = "Character | PowerMode")
	TSubclassOf<AActor> PunchVolumeType;
	UPROPERTY()
	UBoxComponent* PunchVolume;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | PowerMode")
	TSubclassOf<class UCameraShake> PunchShake;
	UPROPERTY()
	TArray<AActor*> HitActors;
	UPROPERTY(EditAnywhere, Category = "Character | PowerMode")
	float RightPunchVolumeYPosition;
	UPROPERTY(EditAnywhere, Category = "Character | PowerMode")
	float LeftPunchVolumeYPosition;
	UPROPERTY()
	AActor* PunchVolumeActor;
	// How far the player should "dash" forward each punch. 0 = don't move at all, 1 = Equivalent to holding down W for roughly 0.2 ms. 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | PowerMode")
	float PunchForwardDistance{0.15f};
	// Amount of damage the punch will do on each hit.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | PowerMode")
	float PunchDamage{50.f};
	UPROPERTY(VisibleAnywhere, Category = "Character | PowerMode")
	float CurrentPowerPoints;
	UPROPERTY(EditAnywhere, Category = "Character | PowerMode")
	float MaxPowerPoints{1.f};
	// How quickly the Power Mode bar drains
	UPROPERTY(EditAnywhere, Category = "Character | PowerMode")
	float DrainSpeed{-0.075f};
	// The frogs health
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | Health")
	float FrogHealth{1000.f};

	UPROPERTY(EditAnywhere, Category = "Character | Shockwave")
	TSubclassOf<AActor> ShockwaveActor;
	UPROPERTY()
	UParticleSystemComponent* ShockwaveSmoke;
	UPROPERTY()
	USphereComponent* ShockwaveCollider;
	UPROPERTY(EditAnywhere, Category = "Character | Shockwave")
	TSubclassOf<class UCameraShake> ShockwaveShake;
	// Blueprint type that should be destroyed when walking on top of it
	UPROPERTY(EditDefaultsOnly, Category = Character)
	TSubclassOf<class ADestructibleObject> SmallDestructible;
	/** The Player's current score */
	UPROPERTY(EditAnywhere, SaveGame, Category = "Character | Score")
	int CurrentScore;
	// How close object has to be to be eaten (destroyed).
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Whirlwind")
	float EatDistance{150.f};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Whirlwind")
	float ShrinkSpeed{0.01f};
	// Blueprint for the Whirlwind mesh or something idk.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Whirlwind")
	float WhirlwindWalkSpeed{600.f};
	// How quickly the object reaches the player.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Whirlwind")
	float SuctionSpeed{500.f};
	// How rapidly the object rotates around the pivot of the whirlwind.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Whirlwind")
	float RotationSpeed{10.f};
	// How quickly the object reaches the middle of the whirlwind.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Whirlwind")
	float InSpeed{30.f};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Whirlwind")
	float MinRadius{5.f};
	UPROPERTY(EditDefaultsOnly, Category = "Character | Whirlwind")
	TSubclassOf<AActor> PivotActor;
	float WhirlwindRange;

	UFUNCTION(BlueprintCallable)
	void PunchAnimNotify();
	void PunchReset();
	UPROPERTY(BlueprintReadWrite)
	uint8 CurrentPunch{0};
	bool bPunchMove{false};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsInWater{false};

	UPROPERTY()
	UBoxComponent* WaterFloor;
protected:

	void BeginPlay() override;
	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);
	void SpawnWhirlwindPfx();

	void Jump() override;
	float InitialZValue;
	void Landed(const FHitResult& Hit) override;

	UPROPERTY(EditDefaultsOnly, Category = "Character | Particles")
	TSubclassOf<AActor> SmokeTrailChild;
	FVector SmokeTrailOffset{0.f, 0.f, -15.f};
	FVector SmokeTrailScale{0.5f, 0.5f, 0.5f};
	FRotator SmokeTrailRot{0.f};
	UPROPERTY(EditDefaultsOnly, Category = "Character | Particles")
	TSubclassOf<AActor> WaterTrailChild;
	FVector WaterTrailOffset{0.f, 0.f, 0.f};
	FVector WaterTrailScale{1.f, 1.f, 1.f};
	FRotator WaterTrailRot{0.f, -90.f, 0.f};
	UPROPERTY()
	AActor* CurrentTrail;

	void SpawnTrail(TSubclassOf<AActor> TrailType, FVector Offset, FVector Scale, const FRotator Rotation);

	void DisableTrail();
	// APawn interface
	void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface
	UFUNCTION(BlueprintImplementableEvent)
	void WhirlwindEvent(bool bStarted);
private:

	float ShockwaveColliderRadius;
	bool bFirstJump{false};
	void Attack();
	void EndAttack();
	void ConstructNeutralModeSettings();
	void AttachedActorsSetup();
	void FilterOccludedObjects();
	void Whirlwind();
	void DoWhirlwind(float DeltaTime);
	void EndWhirlwind();
	void Consume_Impl(AActor* OtherActor);
	bool bUsingWhirlwind{false};
	UPROPERTY(EditAnywhere, Category = "Character | Whirlwind")
	TSubclassOf<AActor> BPWhirlwindPFX;
	UPROPERTY()
	AActor* WhirlwindPFX;
	UPROPERTY()
	FSwirlInfo DefaultWhirlwindSwirl;
	UPROPERTY()
	TArray<AActor*> WhirlwindAffectedActors;
	UPROPERTY()
	TArray<AActor*> PotentialTargets;

	/** Uses fist to punch something, can only be used in power mode **/
	void Punch();
	void DoPunch();
	void ApplyDamage();
	void StopPunch();
	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	               int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                  int32 OtherBodyIndex);
	UFUNCTION()
	void OnAttackOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                     int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnWhirlwindBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	                             UPrimitiveComponent* OtherComp,
	                             int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnWhirlwindEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                           int32 OtherBodyIndex);

	void OpenPauseMenu();
	void SaveGame();

	void LoadGame();
	/** Changing to PowerMode **/
	void PowerMode();
	void SetPlayerModel(const FCharacterSettings& CharacterSettings);
	void PowerDrain(float DeltaTime);
	void DeactivatePowerMode();
	void DisableWhirlwindPfx();

public:

	void Tick(float DeltaTime) override;


	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};
