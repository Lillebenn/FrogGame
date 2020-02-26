// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
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
	// Add other variables here based on what we change between modes.
};

UCLASS(config=Game, Abstract)
class AFrogGameCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;
	float BaseBoomRange{500.f};
	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	// Arrow component that is used to spawn the tongue projectile. 
	UPROPERTY(VisibleAnywhere, Category = Character, meta = (AllowPrivateAccess = "true"))
	class UArrowComponent* TongueStart;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* BoxCollider;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* RightHandCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* LeftHandCollision;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Character, meta = (AllowPrivateAccess = "true"))
	FCharacterSettings NeutralModeSettings;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Character, meta = (AllowPrivateAccess = "true"))
	FCharacterSettings PowerModeSettings;

	friend class ATongueProjectile;
public:
	AFrogGameCharacter();


	/** Accessor Function for Current Score */
	UFUNCTION(BlueprintCallable, Category = "Score")
	float GetCurrentScore() const
	{
		return CurrentScore;
	}

	/** Accessor Function for current size tier **/
	UFUNCTION(BlueprintCallable, Category = "Size")
	uint8 GetCurrentSizeTier() const
	{
		return SizeTier;
	}

	/** Accessor function for next size tier **/
	UFUNCTION(BlueprintCallable, Category = "Size")
	uint8 GetNextSizeTier() const
	{
		return SizeTier + 1;
	}

	/** Updates the players score 
	* @Param Score This is the amount to increase the players score by. This should only be positive!
	*/
	UFUNCTION(BlueprintCallable, Category = "Score")
	void UpdateCurrentScore(const float Score)
	{
		CurrentScore = CurrentScore + Score;
	}

	UFUNCTION(BlueprintCallable, Category = "PowerMode")
	float GetCurrentPowerPoints() const
	{
		return CurrentPowerPoints;
	}

	UArrowComponent* GetTongueStart() const
	{
		return TongueStart;
	}

	/**
	* @Param Points This is the amount to increase the players powerpoints by. This should only be positive on objects!
	*/
	UFUNCTION(BlueprintCallable, Category = "PowerMode")
	void UpdatePowerPoints(float Points);

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;


	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	UPROPERTY(BlueprintReadWrite)
	bool bTongueSpawned{false};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	uint8 SizeTier{2};

	// How many size tiers larger the player needs to be compared to the target item in order to eat it.
	UPROPERTY(EditDefaultsOnly)
	uint8 EdibleThreshold{1};

	// How much weight the auto-aim should give to the camera view angle.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AutoAim)
	float MaxAngleScore{1.f};
	// How much weight the auto-aim should give to the distance between target and player.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AutoAim)
	float MaxDistanceScore{1.f};
	// How much better a target an actor needs to be in order to become the new current target. Lower value means it's easier to switch targets by moving or turning the camera.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = AutoAim)
	float AimStickiness{-0.05f};
	// Max euclidean angle before the object's angle score will be nullifying. 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = AutoAim)
	float MaxAngle{75.f};
	float MaxAngleRadians;


	float CurrentTargetScore{0.f};
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ATongueProjectile> TongueBP;
	uint32 NumTongues{0};
	void Consume(AActor* OtherActor, ATongueProjectile* Tongue);
	// Tongue Settings
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Tongue)
	float BaseTongueReturnSpeed{10000.f};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Tongue)
	float BaseTongueSeekSpeed{4500.f};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Tongue)
	float BaseCableWidth{4.f};
	float CurrentCableWidth{0.f};

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = Tongue)
	float TongueReturnSpeed;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = Tongue)
	float TongueSeekSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = PowerMode)
	bool bPowerMode{false};
	// Amount of damage the punch will do on each hit.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PowerMode)
	float PunchDamage{100.f};
	UPROPERTY(VisibleAnywhere, Category = PowerMode)
	float CurrentPowerPoints;
	UPROPERTY(EditAnywhere, Category = PowerMode)
	float MaxPowerPoints{1.f};
	// How quickly the Power Mode bar drains
	UPROPERTY(EditAnywhere, Category = PowerMode)
	float DrainSpeed{-0.075f};
	/** The Players current score */
	UPROPERTY(EditAnywhere, SaveGame, Category = "Score")
	float CurrentScore;


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

	void Landed(const FHitResult& Hit) override;
	// APawn interface
	void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void ClearCurrentTarget();
	// End of APawn interface

private:
	void UpdateCharacterScale(float ScaleDelta);
	void UpdateCharacterMovement(float ScaleDelta);
	void UpdateCameraBoom(float ScaleDelta) const;
	void UpdateAimRange() const;
	void AutoAim();
	float CalcDistanceScore(AActor* Actor) const;
	/**
	 * 	How ideal this actor is to become the current target based on proximity to the middle of the camera view. A lower value is better.
	 *	An angle exactly as large as the max angle will result in (X / X) * MaxAngleScore = MaxAngleScore.
	 */
	float CalcAngleScore(AActor* Actor) const;

	void SpawnTargetingMesh(const TArray<AActor*>& TargetEdibles) const;
	void PositionAimBox();
	/** Uses the tongue to eat something, and then grows **/
	void Lickitung();
	void SpawnTongue(AActor* Target);
	UPROPERTY()
	TArray<AActor*> Targets;
	UPROPERTY()
	AActor* CurrentTarget;

	/** Uses fist to punch something, can only be used in power mode **/
	void Hitmonchan();
	UFUNCTION()
	void OnAttackHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                 FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void OnBoxTraceEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                   int32 OtherBodyIndex);

	void SaveGame();


	void LoadGame();

	/** Lets the frog jump higher by charging a jump **/
	void StartJump();
	void SetHandCollision(class USphereComponent* Collider, FName CollisionProfile);

	/** Modifier for jump **/
	void ChargeJump(float DeltaTime);

	void ExecuteJump();

	/** Changing to Powermode **/
	void PowerMode();
	void SetPlayerModel(const FCharacterSettings& CharacterSettings);
	void PowerDrain(float DeltaTime);
	void DeactivatePowerMode();


	float ScaleAlpha{0.0f};
	bool bScalingUp{false};
	FVector ExtraScaleTotal{0};


	// Jump stuff
	float BaseJump{450};

	float CurrentJump;
	float JumpBonus{450};
	// The speed at which the jump charges to max velocity when holding down spacebar.
	UPROPERTY(EditAnywhere, Category = Jump)
	float ChargeSpeed{1.5f};
	float JumpModifier{0};
	bool bIsCharging{false};

	float BaseMaxWalkSpeed{600.f};

public:

	void Tick(float DeltaTime) override;


	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};
