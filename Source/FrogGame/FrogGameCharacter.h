// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FrogGameCharacter.generated.h"

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
public:
	AFrogGameCharacter();

	/** Accessor Function for Current Score */
	UFUNCTION(BlueprintCallable, Category = "Score")
	float GetCurrentScore();

	/** Updates the players score 
	* @Param Score This is the amount to increase the players score by. This should only be positive!
	*/
	UFUNCTION(BlueprintCallable, Category = "Score")
	void UpdateCurrentScore(float Score);

	UFUNCTION(BlueprintCallable, Category = "PowerMode")
	float GetCurrentPowerPoints();

	/**
	* @Param Points This is the amount to increase the players powerpoints by. This should only be positive on objects!
	*/
	UFUNCTION(BlueprintCallable, Category = "PowerMode")
	void UpdatePowerPoints(float Points);

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	UArrowComponent* GetTongueStart();
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

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ATongueProjectile> Tongue;

	UPROPERTY()
	class UCableComponent* Cable;

	UPROPERTY()
	AActor* CurrentTarget;


	FName BoneTarget;
	FName LastBone;
	void Consume(AActor* OtherActor, FName BoneName = FName());
	// Tongue Settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tongue)
	float BaseTongueInSpeed{10000.f};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tongue)
	float BaseTongueOutSpeed{4500.f};

	float TongueInSpeed;
	float TongueOutSpeed;
	
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


protected:
	// APawn interface
	void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

private:

	/** Updates the frogs length **/
	void UpdateCameraBoom(float ScaleDelta);

	/** Uses the tongue to eat something, and then grows **/
	void Lickitung();

	/** Uses fist to punch something, can only be used in power mode **/
	void Hitmonchan();
	UFUNCTION()
	void OnAttackHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                 FVector NormalImpulse, const FHitResult& Hit);
	UFUNCTION()
	void OnAttackHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                 FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void OnBoxTraceEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                   int32 OtherBodyIndex);

	void SaveGame();


	void LoadGame();

	void GetClosestChunk(class UDestructibleComponent* Component);
	
	/** Lets the frog jump higher by charging a jump **/
	void StartJump();
	void SetHandCollision(class USphereComponent* Collider, FName CollisionProfile);

	/** Modifier for jump **/
	void ChargeJump(float DeltaTime);

	void ExecuteJump();

	/** Changing to Powermode **/
	void PowerMode();

	void PowerDrain(float DeltaTime);


	float ScaleAlpha{0.0f};
	bool bScalingUp{false};
	FVector DesiredScale{0};

	// Jump stuff
	float BaseJump{450};

	UPROPERTY(EditAnywhere, Category = Jump)
	float JumpBonus{450};
	// The speed at which the jump charges to max velocity when holding down spacebar.
	UPROPERTY(EditAnywhere, Category = Jump)
	float ChargeSpeed{1.5f};
	float JumpModifier{0};
	float CurrentJump;
	bool bIsCharging{false};

	float BaseMaxWalkSpeed{600.f};
public:

	void Tick(float DeltaTime) override;
	void AutoAim();

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};
