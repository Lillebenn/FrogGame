// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Components/SphereComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TimerManager.h"
#include "FFrogLibrary.h"
#include "Sound/SoundCue.h"

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
	/** Overlap volume to check for possible whirlwind targets */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* WhirlwindVolume;

	UPROPERTY (VisibleAnywhere, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	class UParticleSystemComponent* FireEyeOne;
	UPROPERTY (VisibleAnywhere, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	class UParticleSystemComponent* FireEyeTwo;
	UPROPERTY (VisibleAnywhere, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	class UParticleSystemComponent* PowerUpParticleSystem;


public:
	AFrogGameCharacter();
	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;
	/** Updates the players score 
	* @Param Score This is the amount to increase the players score by. This should only be positive!
	*/
	UFUNCTION(BlueprintCallable, Category = "Character | Score")
	void UpdateCurrentScore(int Score);
	/**
	* @Param Points This is the amount to increase the player's power points by. This should only be positive on objects!
	*/
	UFUNCTION(BlueprintCallable, Category = "Character | PowerMode")
	void UpdatePowerPoints(float Points);

	UFUNCTION(BlueprintCallable, Category = "Character | PowerMode")
	bool CanTransform() const
	{
		return CurrentPowerPoints >= MaxPowerPoints / 10.f;
	}

	ECharacterMode GetCurrentMode() const
	{
		return CurrentMode;
	}

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | PowerMode")
	bool bPowerMode{false};


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | PowerMode")
	float CurrentPowerPoints{0.f};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character | PowerMode", meta = (EditCondition=
		"bIsDefaultPowerBlueprint"))
	float MaxPowerPoints{400.f};

	// The amount of "frog children" collected.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | Objective")
	int FrogsCollected{0};
	// How many "frog children" are needed to win the game.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character | Objective", meta = (EditCondition="!bIsDefaultPowerBlueprint"))
	int TotalFrogChildren{10};


	// Type of blueprint object that should be destroyed when walking on top of it.
	UPROPERTY(EditDefaultsOnly, Category = Character, meta = (EditCondition="!bIsDefaultPowerBlueprint"))
	TSubclassOf<class ADestructibleObject> SmallDestructible;

	/** The Player's current score */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | Score")
	int CurrentScore{0};

	// How much of the world, percentage-wise, has been destroyed (this is not accurate).
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | Score")
	float DestroyedPercent{0.f};

	UFUNCTION(BlueprintCallable)
	void IncreaseGravity() const;
	UFUNCTION(BlueprintCallable, Category = Animation)
	void PunchAnimNotify(FName Socket);
	UFUNCTION(BlueprintCallable, Category = Animation)
	void PunchResetNotify();
	UFUNCTION(BlueprintCallable, Category = Animation)
	void PunchStopNotify();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
	bool bIsInWater{false};
	/**
	* Generic function for taking actors that contain an EdibleComponent, getting score and power points from it, and then destroying the actor.
	* @param OtherActor should be an actor containing an EdibleComponent.
	*/
	void Consume(AActor* OtherActor);
	/**
	 * Specific function for the spheres that spawn after an object is destroyed by punching it.
	 */
	void Consume(class ASphereDrop* Sphere);

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
	/**
	 * Overridden to activate the shockwave collider so that we can check it when we land.
	 */
	void Jump() override;

	/**
	 * Overridden to reset the gravity scale to normal, and apply shockwave damage.
	 */
	void Landed(const FHitResult& Hit) override;

	void TestFunction();


	// APawn interface
	void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface
	/**
	 * Triggers an event in blueprint to open the frog's mouth over time.
	 */
	UFUNCTION(BlueprintImplementableEvent)
	void WhirlwindEvent(bool bStarted);
	/**
	 * Triggers an event in blueprint to play our custom in-game particles.
	 */
	UFUNCTION(BlueprintImplementableEvent)
	void ActivatePowerupPFX();
	/**
	 * Called to reduce the player's score.
	 * @param DamageAmount Positive values subtract this amount from the player score.
	 */
	UFUNCTION(BlueprintCallable)
	void TakeScoreDamage(float DamageAmount);
private:
	UPROPERTY()
	AFrogGameCharacter* NeutralModeSettings;
	// Tick this if this is just a template we switch to during power mode.
	UPROPERTY(EditDefaultsOnly, Category = Character)
	bool bIsDefaultPowerBlueprint{false};
	UPROPERTY(EditDefaultsOnly, Category = "Character | PowerMode", meta = (EditCondition=
		"!bIsDefaultPowerBlueprint", DisplayPriority="1"))
	TSubclassOf<AFrogGameCharacter> PowerModeBP;
	UPROPERTY()
	AFrogGameCharacter* PowerModeSettings;
	bool bCameraZoom{false};
	float DesiredCamDistance;
	UPROPERTY()
	TSubclassOf<AFrogGameCharacter> NeutralModeBP;
	// InGameHUD
	UPROPERTY()
	class UFrogGameUI* FrogHUD;
	bool bPressRVisible{false};
	UPROPERTY()
	class AFrogGameMode* GameMode;
	UPROPERTY()
	UAudioComponent* PlayingWhirlwindSound;
	UPROPERTY()
	UAudioComponent* RegAmbientSoundComponent;
	UPROPERTY()
	UAudioComponent* SeaAmbientSoundComponent;

	float ShockwaveColliderRadius;
	float InitialZValue;
	UPROPERTY()
	USphereComponent* ShockwaveCollider;
	UPROPERTY()
	UBoxComponent* WaterFloorCollider;
	UPROPERTY()
	AActor* WaterBreakActor;
	void SpawnWaterBreak();
	void DisableWaterBreak();

	bool bJumped{false};
	bool bOnConcrete{false};

	FVector RegularBoxExtent;
	ECharacterMode CurrentMode{ECharacterMode::Neutral};
	float WalkSpeed;

	// Currently set to an arbitrary value 
	UPROPERTY(EditDefaultsOnly, Category = "Character | Score", meta = (EditCondition="!bIsDefaultPowerBlueprint"))
	int NumObjectsInGame{70000};
	// How much less a power point should be worth during Power mode, to stop the player from being in that mode indefinitely.
	UPROPERTY(EditDefaultsOnly, Category = "Character | PowerMode", meta = (EditCondition="bIsDefaultPowerBlueprint"))
	float PowerPointsDivisor{3.f};
	// How quickly the Power mode bar drains.
	UPROPERTY(EditDefaultsOnly, Category = "Character | PowerMode", meta = (EditCondition="bIsDefaultPowerBlueprint"))
	float DrainSpeed{-10.f};

	UPROPERTY(VisibleAnywhere, Category = "Character | PowerMode")
	bool bInfinitePower{false};


	UPROPERTY (EditDefaultsOnly, Category = Character, meta = (EditCondition="bIsDefaultPowerBlueprint"))
	class UAnimMontage* PunchMontage;
	UPROPERTY(EditAnywhere, Category = "Character | PowerMode", meta = (EditCondition="bIsDefaultPowerBlueprint"))
	TSubclassOf<AActor> PunchVolumeType;
	UPROPERTY()
	AActor* PunchVolumeActor;
	UPROPERTY()
	UBoxComponent* PunchVolume;
	UPROPERTY(EditAnywhere, Category = "Character | PowerMode", meta = (EditCondition=
		"bIsDefaultPowerBlueprint"))
	TSubclassOf<class UCameraShake> PunchShake;
	UPROPERTY(EditAnywhere, Category = "Character | Particles", meta = (EditCondition=
		"bIsDefaultPowerBlueprint"))
	FVector PunchOnePFXOffset{0.f, 8.f, 0.f};
	UPROPERTY(EditAnywhere, Category = "Character | Particles", meta = (EditCondition=
		"bIsDefaultPowerBlueprint"))
	FVector PunchTwoPFXOffset{0.f, -8.f, 0.f};
	UPROPERTY(EditAnywhere, Category = "Character | Particles", meta = (EditCondition=
		"bIsDefaultPowerBlueprint"))
	FVector UpperCutPFXOffset{0.f, 0.f, 0.f};
	UPROPERTY(VisibleAnywhere, Category = "Character | PowerMode")
	bool bIsPunching{false};
	FTimerHandle PunchRepeatTimer;

	int CurrentDestroyedNum{0};
	/**
	 * Get the current amount of destroyed objects and compare it to the total number of objects, getting a percentage value from that.
	 */
	void UpdateDestroyedPercent();
	bool bPunchMove{false};
	// How far the player should "dash" forward each punch. 0 = don't move at all, 1 = Equivalent to holding down W for roughly 0.2 ms. 
	UPROPERTY(EditAnywhere, Category = "Character | PowerMode", meta = (EditCondition=
		"bIsDefaultPowerBlueprint"))
	float PunchForwardDistance{1.f};
	// Amount of damage the punch will do on each hit.
	UPROPERTY(EditAnywhere, Category = "Character | PowerMode", meta = (EditCondition=
		"bIsDefaultPowerBlueprint"))
	float PunchDamage{500.f};


	uint8 CurrentPunch{0};

	UPROPERTY(EditAnywhere, Category = "Character | PowerMode", meta = (EditCondition="bIsDefaultPowerBlueprint"))
	float RightPunchVolumeYPosition{0.f};
	UPROPERTY(EditAnywhere, Category = "Character | PowerMode", meta = (EditCondition="bIsDefaultPowerBlueprint"))
	float RightPunchVolumeZPosition{0.f};
	UPROPERTY(EditAnywhere, Category = "Character | PowerMode", meta = (EditCondition="bIsDefaultPowerBlueprint"))
	float LeftPunchVolumeYPosition{0.f};
	UPROPERTY(EditAnywhere, Category = "Character | PowerMode", meta = (EditCondition="bIsDefaultPowerBlueprint"))
	float LeftPunchVolumeZPosition{0.f};
	UPROPERTY(EditAnywhere, Category = "Character | PowerMode", meta = (EditCondition="bIsDefaultPowerBlueprint"))
	float UpperCutVolumeZPosition{0.f};
	UPROPERTY(EditAnywhere, Category = "Character | PowerMode", meta = (EditCondition="bIsDefaultPowerBlueprint"))
	float UpperCutVolumeYPosition{0.f};
	// Array of actors that were hit by the latest punch.
	UPROPERTY()
	TArray<AActor*> HitActors;

	// Particles to play at the end of a successful punch hit.
	UPROPERTY (EditAnywhere, Category = "Character | Particles", meta = (EditCondition=
		"bIsDefaultPowerBlueprint"))
	TArray<class UParticleSystem*> PunchParticleSystems;
	int PunchParticleIndex{0};

	UParticleSystem* GetNextPunchParticle()
	{
		const int CurParticleIndex{PunchParticleIndex};
		PunchParticleIndex++;
		if (PunchParticleIndex >= PunchParticleSystems.Num())
		{
			PunchParticleIndex = 0;
		}
		return PunchParticleSystems[CurParticleIndex];
	}

	UPROPERTY(EditAnywhere, Category = Character)
	float SwimSpeed{2300.f};
	void Swim(bool bActivate);
	// Actor containing a particle system component that plays while the player is moving in water.
	UPROPERTY(EditDefaultsOnly, Category = "Character | Particles", meta = (EditCondition="!bIsDefaultPowerBlueprint"))
	TSubclassOf<AActor> WaterBreakType;
	UPROPERTY(EditAnywhere, Category = "Character | Particles")
	FVector WaterBreakOffset{0.f};
	UPROPERTY(EditAnywhere, Category = "Character | Particles")
	FVector WaterBreakScale{1.f, 1.f, 1.f};
	FRotator WaterBreakRot{0.f, -90.f, 0.f};

	// Actor containing a sphere collider to check for overlaps when landing.
	UPROPERTY(EditAnywhere, Category = "Character | Shockwave", meta = (EditCondition="!bIsDefaultPowerBlueprint"))
	TSubclassOf<AActor> ShockwaveActor;
	// Particle to play when the player lands in the water.
	UPROPERTY (EditDefaultsOnly, Category = "Character | Shockwave", meta = (EditCondition=
		"!bIsDefaultPowerBlueprint"))
	class UParticleSystem* WaterShockwave;
	// Particle to play when the player lands on the ground, if a shockwave is activated.
	UPROPERTY (EditDefaultsOnly, Category = "Character | Shockwave", meta = (EditCondition=
		"!bIsDefaultPowerBlueprint"))
	class UParticleSystem* LandShockwave;
	// Size of the water shockwave particle system.
	UPROPERTY (EditDefaultsOnly, Category = "Character | Shockwave")
	float WaterShockwaveScale{0.15f};
	// Size of the ground shockwave particle system.
	UPROPERTY (EditDefaultsOnly, Category = "Character | Shockwave", meta = (EditCondition="bIsDefaultPowerBlueprint"))
	float LandShockwaveScale{0.5f};

	// Type of camera shake to play when landing for a shockwave (any type).
	UPROPERTY(EditAnywhere, Category = "Character | Shockwave", meta = (EditCondition="!bIsDefaultPowerBlueprint"))
	TSubclassOf<class UCameraShake> ShockwaveShake;

	UFUNCTION()
	void InfinitePower();
	/**
	 * Constructs any actors attached to this.
	 */
	void AttachedActorsSetup();
	/**
	 * Start Whirlwind or Punch depending on bPowerMode.
	 */
	void Attack();
	/**
	 * Stop Whirlwind and/or Punch.
	 */
	void EndAttack();

	/**
	 * Consume implementation that destroys the actor after use.
	 */
	void Consume_Impl(AActor* OtherActor);
	/**
	 * Simple Consume implementation that doesn't destroy the actor afterwards.
	 */
	void Consume_Impl(float PowerPoints, float ScorePoints);

	void FilterOccludedObjects();
	void Whirlwind();
	void DoWhirlwind(float DeltaTime);
	void EndWhirlwind();
	float WhirlwindRange;
	bool bUsingWhirlwind{false};
	bool bAteSomething{false};
	UPROPERTY(EditAnywhere, Category = "Character | Whirlwind", meta = (EditCondition="!bIsDefaultPowerBlueprint"))
	TSubclassOf<AActor> BPWhirlwindPFX;
	UPROPERTY(EditDefaultsOnly, Category = "Character | Whirlwind", meta = (EditCondition="!bIsDefaultPowerBlueprint"))
	TSubclassOf<AActor> PivotActor;
	UPROPERTY()
	AActor* WhirlwindPFX;
	UPROPERTY()
	FSwirlInfo DefaultWhirlwindSwirl;
	UPROPERTY()
	TArray<AActor*> WhirlwindAffectedActors;
	UPROPERTY()
	TArray<AActor*> PotentialTargets;
	// How close object has to be to be eaten (destroyed).
	UPROPERTY(EditDefaultsOnly, Category = "Character | Whirlwind", meta = (EditCondition=
		"!bIsDefaultPowerBlueprint"))
	float EatDistance{150.f};
	// How fast objects should shrink once they're in range for it.
	UPROPERTY(EditDefaultsOnly, Category = "Character | Whirlwind", meta = (EditCondition=
		"!bIsDefaultPowerBlueprint"))
	float ShrinkSpeed{0.01f};

	UPROPERTY(EditDefaultsOnly, Category = "Character | Whirlwind", meta = (EditCondition=
		"!bIsDefaultPowerBlueprint"))
	float WhirlwindWalkSpeed{600.f};

	// How quickly the object reaches the player.
	UPROPERTY(EditDefaultsOnly, Category = "Character | Whirlwind", meta = (EditCondition=
		"!bIsDefaultPowerBlueprint"))
	float SuctionSpeed{500.f};
	// How rapidly the object rotates around the pivot of the whirlwind.
	UPROPERTY(EditDefaultsOnly, Category = "Character | Whirlwind", meta = (EditCondition=
		"!bIsDefaultPowerBlueprint"))
	float RotationSpeed{10.f};
	// How quickly the object reaches the middle of the whirlwind.
	UPROPERTY(EditDefaultsOnly, Category = "Character | Whirlwind", meta = (EditCondition=
		"!bIsDefaultPowerBlueprint"))
	float InSpeed{30.f};
	// Determines the minimum radius with respect to the center of the whirlwind.
	UPROPERTY(EditDefaultsOnly, Category = "Character | Whirlwind", meta = (EditCondition=
		"!bIsDefaultPowerBlueprint"))
	float MinRadius{5.f};

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
	UPROPERTY(EditDefaultsOnly, meta = (EditCondition="!bIsDefaultPowerBlueprint"))
	TSubclassOf<AActor> CullingObjectsType;
	UPROPERTY()
	AActor* CullingActorObjects;
	UPROPERTY()
	USphereComponent* CullingObjectsSphere;
	// We cull ADestructible actors by distance, by setting them to Mobility = Static by default, and flipping to Movable when we get within reach of them.
	UFUNCTION()
	void OnCullingObjectsOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	                             UPrimitiveComponent* OtherComp,
	                             int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnCullingObjectsEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	                                UPrimitiveComponent* OtherComp,
	                                int32 OtherBodyIndex);


	/**
	 * Sets ADestructible actors within reach to Movable when we click Play (or any other time this might be prudent).
	 */
	UFUNCTION(BlueprintCallable)
	void OnHitPlay() const;
	void OpenPauseMenu();
	/**
	 *	Activates Power mode.
	*/
	void PowerMode();
	FTimerHandle PowerModeDelay;
	/**
	 * Switches to the model we use for the Power mode, with any other settings might be needed.
	 */
	void ActivatePowerModel();
	/**
	 * Generic function for setting the player model and other settings related to it.
	 * @param CharacterSettings is a copy of each of the two blueprints we use to switch between modes.
	 */
	void SetPlayerModel(AFrogGameCharacter* CharacterSettings);
	/**
	 * Copies the blueprint based on this class, and another that has bIsDefaultPowerBlueprint ticked,
	 * which gives access to a bunch of editable blueprint variables that are then copied in this function onto the real blueprint.
	 */
	void SetupSettingsCopies();
	/**
	 * Drains power from the player when they're in Power mode.
	 */
	void PowerDrain(float DeltaTime);
	/**
	 * Deactivates the Power mode and resets to the original blueprint values.
	 */
	void DeactivatePowerMode();
	/**
	 * Turns off the whirlwind particle effect.
	 */
	void DisableWhirlwindPfx();

	// --- Sounds --- //

	// Sounds to play when the player lands in the water.
	UPROPERTY(EditDefaultsOnly, Category = "Character | Sound", meta = (EditCondition=
		"!bIsDefaultPowerBlueprint"))
	TArray<USoundCue*> SplashSounds;
	FORCEINLINE bool HasSplashSounds() const
	{
		return SplashSounds.Num() > 0;
	}

	UFUNCTION(BlueprintCallable)
	FORCEINLINE USoundCue* GetSplashSound() const
	{
		return FFrogLibrary::GetRandomSoundByArray(SplashSounds);
	}

	// Sound to play when the player lands on the ground.
	UPROPERTY(EditDefaultsOnly, Category = "Character | Sound", meta = (EditCondition="!bIsDefaultPowerBlueprint"))
	USoundCue* LandingSound;

	// Sound to play when the player powers up.
	UPROPERTY(EditDefaultsOnly, Category = "Character | Sound", meta = (EditCondition=
		"!bIsDefaultPowerBlueprint"))
	USoundCue* PowerUpTransition;

	// Sounds to play when player punches and hits nothing.
	UPROPERTY(EditDefaultsOnly, Category = "Character | Sound", meta = (EditCondition=
		"!bIsDefaultPowerBlueprint"))
	TArray<USoundCue*> PunchAirSounds;
	FORCEINLINE bool HasPunchAirSounds() const
	{
		return PunchAirSounds.Num() > 0;
	}

	UFUNCTION(BlueprintCallable)
	FORCEINLINE USoundCue* GetPunchAirSound() const
	{
		return FFrogLibrary::GetRandomSoundByArray(PunchAirSounds);
	}

	// Sounds to play when the player punches and hits something.
	UPROPERTY(EditDefaultsOnly, Category = "Character | Sound", meta = (EditCondition=
		"!bIsDefaultPowerBlueprint"))
	TArray<USoundCue*> PunchHitSounds;
	FORCEINLINE bool HasPunchHitSounds() const
	{
		return PunchHitSounds.Num() > 0;
	}

	UFUNCTION(BlueprintCallable)
	FORCEINLINE USoundCue* GetPunchHitSound() const
	{
		return FFrogLibrary::GetRandomSoundByArray(PunchHitSounds);
	}

	void PlayPunchSounds(bool bSuccess) const;
	// Sounds to play when the player walks on regular ground.
	UPROPERTY(EditDefaultsOnly, Category = "Character | Sound", meta = (EditCondition=
		"!bIsDefaultPowerBlueprint"))
	TArray<USoundCue*> RegularFootstepSounds;
	// Sounds to play when the player walks on concrete (in the cities).
	UPROPERTY(EditDefaultsOnly, Category = "Character | Sound", meta = (EditCondition=
		"!bIsDefaultPowerBlueprint"))
	TArray<USoundCue*> ConcreteFootstepSounds;


	UFUNCTION(BlueprintCallable)
	FORCEINLINE USoundCue* GetFootStepSound() const
	{
		if (bOnConcrete)
		{
			return FFrogLibrary::GetRandomSoundByArray(ConcreteFootstepSounds);
		}
		return FFrogLibrary::GetRandomSoundByArray(RegularFootstepSounds);
	}

	// Explosion sounds played when taking damage from tanks or helicopter.
	UPROPERTY(EditDefaultsOnly, Category = "Character | Sound", meta = (EditCondition=
		"!bIsDefaultPowerBlueprint"))
	TArray<USoundCue*> DamageTakenSounds;
	FORCEINLINE USoundCue* GetDamageTakenSound() const
	{
		return FFrogLibrary::GetRandomSoundByArray(DamageTakenSounds);
	}

	// Sound to play when the player uses the whirlwind ability.
	UPROPERTY(EditDefaultsOnly, Category = "Character | Sound", meta = (EditCondition=
		"!bIsDefaultPowerBlueprint"))
	USoundBase* WhirlwindSound;
	// Sound to play when the player ends the whirlwind ability and has managed to eat something.
	UPROPERTY(EditDefaultsOnly, Category = "Character | Sound", meta = (EditCondition=
		"!bIsDefaultPowerBlueprint"))
	USoundCue* EatSound;

	// Ambient sound to play when the player is not near the sea.
	UPROPERTY(EditDefaultsOnly, Category = "Character | Sound", meta = (EditCondition=
		"!bIsDefaultPowerBlueprint"))
	USoundBase* RegAmbientSound;

	// Sound to play when the player is near the sea.
	UPROPERTY(EditDefaultsOnly, Category = "Character | Sound", meta = (EditCondition=
		"!bIsDefaultPowerBlueprint"))
	USoundBase* SeaAmbientSound;


public:

	void Tick(float DeltaTime) override;


	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};
