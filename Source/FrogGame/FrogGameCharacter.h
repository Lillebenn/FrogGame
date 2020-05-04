// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Components/SphereComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TimerManager.h"
#include "FFrogLibrary.h"
#include "SphereDrop.h"
#include "FrogGameCharacter.generated.h"

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


	UPROPERTY (VisibleAnywhere, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	class UParticleSystemComponent* FireEyeOne;
	UPROPERTY (VisibleAnywhere, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	class UParticleSystemComponent* FireEyeTwo;
	UPROPERTY (VisibleAnywhere, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	class UParticleSystemComponent* PowerUpParticleSystem;
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
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character | Score")
	int CheckpointScoreReq{500000};
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
		/** Accessor Function for Frogs Collected */
	UFUNCTION(BlueprintCallable, Category = "Character | Objective")
	int GetCurrentFrogsCollected() const
	{
		return FrogsCollected;
	}
	UFUNCTION(BlueprintCallable, Category = "Character | PowerMode")
	bool CanTransform() const
	{
		return CurrentPowerPoints >= MaxPowerPoints / 10.f;
	}



	UPROPERTY()
	AFrogGameCharacter* NeutralModeSettings;
	UPROPERTY()
	TSubclassOf<AFrogGameCharacter> NeutralModeBP;

	// Tick this if this is just a template we switch to during powermode.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Character)
	bool bIsDefaultPowerBlueprint{false};
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character | PowerMode", meta = (AllowPrivateAccess =
		"true", EditCondition="!bIsDefaultPowerBlueprint"))
	TSubclassOf<AFrogGameCharacter> PowerModeBP;
	UPROPERTY()
	AFrogGameCharacter* PowerModeSettings;
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
	UPROPERTY (EditAnywhere, BlueprintReadWrite, Category = "Character | Particles", meta = (EditCondition=
		"bIsDefaultPowerBlueprint"))
	class UParticleSystem* PunchOne;
	UPROPERTY (EditAnywhere, BlueprintReadWrite, Category = "Character | Particles", meta = (EditCondition=
		"bIsDefaultPowerBlueprint"))
	class UParticleSystem* PunchTwo;
	UPROPERTY (EditAnywhere, BlueprintReadWrite, Category = "Character | Particles", meta = (EditCondition=
		"bIsDefaultPowerBlueprint"))
	class UParticleSystem* UpperCut;
	UPROPERTY (EditDefaultsOnly, BlueprintReadOnly, Category = Character, meta = (
		EditCondition="bIsDefaultPowerBlueprint"))
	class UAnimMontage* PunchMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | PowerMode", meta = (EditCondition=
		"bIsDefaultPowerBlueprint"))
	FVector PunchOneOffset{0.f, 8.f, 0.f};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | PowerMode", meta = (EditCondition=
		"bIsDefaultPowerBlueprint"))
	FVector PunchTwoOffset{0.f, -8.f, 0.f};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | PowerMode", meta = (EditCondition=
		"bIsDefaultPowerBlueprint"))
	FVector UpperCutOffset{0.f, 0.f, 0.f};
	FVector RegularBoxExtent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | PowerMode", meta = (EditCondition=
		"bIsDefaultPowerBlueprint"))
	FVector UpperCutBoxExtent{60.f, 30.f, 80.f};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | PowerMode")
	bool bPowerMode{false};
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character | PowerMode")
	bool bIsPunching{false};
	UPROPERTY(EditAnywhere, Category = "Character | PowerMode")
	bool bInfinitePower{false};
	UFUNCTION()
	void InfinitePower();
	UPROPERTY(EditAnywhere, Category = "Character | PowerMode", meta = (EditCondition="bIsDefaultPowerBlueprint"))
	TSubclassOf<AActor> PunchVolumeType;
	UPROPERTY()
	UBoxComponent* PunchVolume;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | PowerMode", meta = (EditCondition=
		"bIsDefaultPowerBlueprint"))
	TSubclassOf<class UCameraShake> PunchShake;
	UPROPERTY()
	TArray<AActor*> HitActors;
	UPROPERTY(EditAnywhere, Category = "Character | PowerMode", meta = (EditCondition="bIsDefaultPowerBlueprint"))
	float RightPunchVolumeYPosition{24.f};
	UPROPERTY(EditAnywhere, Category = "Character | PowerMode", meta = (EditCondition="bIsDefaultPowerBlueprint"))
	float LeftPunchVolumeYPosition{-45.f};
	UPROPERTY()
	AActor* PunchVolumeActor;
	// How far the player should "dash" forward each punch. 0 = don't move at all, 1 = Equivalent to holding down W for roughly 0.2 ms. 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | PowerMode", meta = (EditCondition=
		"bIsDefaultPowerBlueprint"))
	float PunchForwardDistance{1.f};
	// Amount of damage the punch will do on each hit.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | PowerMode", meta = (EditCondition=
		"bIsDefaultPowerBlueprint"))
	float PunchDamage{500.f};

	UPROPERTY(VisibleAnywhere, Category = "Character | PowerMode")
	float CurrentPowerPoints{0.f};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character | PowerMode", meta = (EditCondition=
		"bIsDefaultPowerBlueprint"))
	float MaxPowerPoints{400.f};
	// How much less a power point should be worth during powermode, to stop the player from being in that mode indefinitely.
	UPROPERTY(EditAnywhere, Category = "Character | PowerMode", meta = (EditCondition="bIsDefaultPowerBlueprint"))
	float PowerPointsDivisor{3.f};
	// How quickly the Power Mode bar drains
	UPROPERTY(EditAnywhere, Category = "Character | PowerMode", meta = (EditCondition="bIsDefaultPowerBlueprint"))
	float DrainSpeed{-5.f};
	// The ammount of frogs collected
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | Objective")
	int FrogsCollected{0};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | Objective")
	int TotalFrogChildren{10};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Character)
	float SwimSpeed{2300.f};
	float WalkSpeed;
	UPROPERTY(EditAnywhere, Category = "Character | Shockwave", meta = (EditCondition="!bIsDefaultPowerBlueprint"))
	TSubclassOf<AActor> ShockwaveActor;
	UPROPERTY (EditDefaultsOnly, BlueprintReadWrite, Category = "Character | Shockwave", meta = (EditCondition=
		"!bIsDefaultPowerBlueprint"))
	class UParticleSystem* WaterShockwave;

	UPROPERTY (EditDefaultsOnly, BlueprintReadWrite, Category = "Character | Shockwave", meta = (EditCondition=
		"!bIsDefaultPowerBlueprint"))
	class UParticleSystem* LandShockwave;
	UPROPERTY (EditDefaultsOnly, BlueprintReadWrite, Category = "Character | Shockwave")
	float WaterShockwaveScale{0.15f};
	UPROPERTY (EditDefaultsOnly, BlueprintReadWrite, Category = "Character | Shockwave")
	float LandShockwaveScale{0.5f};

	UPROPERTY()
	USphereComponent* ShockwaveCollider;
	UPROPERTY(EditAnywhere, Category = "Character | Shockwave", meta = (EditCondition="!bIsDefaultPowerBlueprint"))
	TSubclassOf<class UCameraShake> ShockwaveShake;
	// Blueprint type that should be destroyed when walking on top of it
	UPROPERTY(EditDefaultsOnly, Category = Character, meta = (EditCondition="!bIsDefaultPowerBlueprint"))
	TSubclassOf<class ADestructibleObject> SmallDestructible;
	int LastCheckpointScore{0};
	/** The Player's current score */
	UPROPERTY(EditAnywhere, Category = "Character | Score")
	int CurrentScore;
	// How close object has to be to be eaten (destroyed).
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Whirlwind", meta = (EditCondition=
		"!bIsDefaultPowerBlueprint"))
	float EatDistance{150.f};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Whirlwind", meta = (EditCondition=
		"!bIsDefaultPowerBlueprint"))
	float ShrinkSpeed{0.01f};
	// Blueprint for the Whirlwind mesh or something idk.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Whirlwind", meta = (EditCondition=
		"!bIsDefaultPowerBlueprint"))
	float WhirlwindWalkSpeed{600.f};
	// How quickly the object reaches the player.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Whirlwind", meta = (EditCondition=
		"!bIsDefaultPowerBlueprint"))
	float SuctionSpeed{500.f};
	// How rapidly the object rotates around the pivot of the whirlwind.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Whirlwind", meta = (EditCondition=
		"!bIsDefaultPowerBlueprint"))
	float RotationSpeed{10.f};
	// How quickly the object reaches the middle of the whirlwind.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Whirlwind", meta = (EditCondition=
		"!bIsDefaultPowerBlueprint"))
	float InSpeed{30.f};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Whirlwind", meta = (EditCondition=
		"!bIsDefaultPowerBlueprint"))
	float MinRadius{5.f};
	UPROPERTY(EditDefaultsOnly, Category = "Character | Whirlwind", meta = (EditCondition="!bIsDefaultPowerBlueprint"))
	TSubclassOf<AActor> PivotActor;
	float WhirlwindRange;
	UFUNCTION(BlueprintCallable)
	void IncreaseGravity() const;
	UFUNCTION(BlueprintCallable)
	void PunchOneAnimNotify();
	UFUNCTION(BlueprintCallable)
	void PunchTwoAnimNotify();
	UFUNCTION(BlueprintCallable)
	void UpperCutAnimNotify();
	UFUNCTION(BlueprintCallable)
	void PunchResetNotify();
	UFUNCTION(BlueprintCallable)
	void PunchStopNotify();

	UPROPERTY(BlueprintReadWrite)
	uint8 CurrentPunch{0};
	bool bPunchMove{false};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
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

	UPROPERTY(EditDefaultsOnly, Category = "Character | Particles", meta = (EditCondition="!bIsDefaultPowerBlueprint"))
	TSubclassOf<AActor> SmokeTrailChild;
	FVector SmokeTrailOffset{0.f, 0.f, -15.f};
	FVector SmokeTrailScale{0.5f, 0.5f, 0.5f};
	FRotator SmokeTrailRot{0.f};
	UPROPERTY(EditDefaultsOnly, Category = "Character | Particles", meta = (EditCondition="!bIsDefaultPowerBlueprint"))
	TSubclassOf<AActor> WaterBreakChild;
	UPROPERTY(EditAnywhere, Category = "Character | Particles")
	FVector WaterBreakOffset{0.f};
	UPROPERTY(EditAnywhere, Category = "Character | Particles")
	FVector WaterBreakScale{1.f, 1.f, 1.f};
	FRotator WaterBreakRot{0.f, -90.f, 0.f};
	UPROPERTY()
	AActor* CurrentTrail;
	UPROPERTY(EditAnywhere, Category = "Character | Particles")
	bool bTestTrail{false};
	void TestTrail();
	UFUNCTION()
	void PauseMontage();
	bool bShouldPauseMontage{false};
	void SpawnTrail(TSubclassOf<AActor> TrailType, FVector Offset, FVector Scale, FRotator Rotation);

	void DisableTrail();
	// APawn interface
	void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface
	UFUNCTION(BlueprintImplementableEvent)
	void WhirlwindEvent(bool bStarted);
	UFUNCTION(BlueprintImplementableEvent)
	void ActivatePowerupPFX();

private:
	float ShockwaveColliderRadius;
	bool bJumped{false};
	void Attack();
	void EndAttack();
	void AttachedActorsSetup();
	void FilterOccludedObjects();
	void Whirlwind();
	void DoWhirlwind(float DeltaTime);
	void EndWhirlwind();
	void Consume_Impl(AActor* OtherActor);
	bool bUsingWhirlwind{false};
	UPROPERTY(EditAnywhere, Category = "Character | Whirlwind", meta = (EditCondition="!bIsDefaultPowerBlueprint"))
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
	FTimerHandle NextPunchTimer;
	void QueuedPunch();
	bool bAttackHeld{false};
	void ApplyDamage();
	void StopPunch();
	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	               int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                  int32 OtherBodyIndex);
	void Swim(bool bActivate);
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
	/** Changing to PowerMode **/
	void PowerMode();
	FTimerHandle PowerModeDelay;
	void ActivatePowerModel();
	void SetPlayerModel(AFrogGameCharacter* CharacterSettings);
	void SetupSettingsCopies();
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
