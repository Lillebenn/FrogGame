// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TimerManager.h"
#include "EdibleComponent.h"
#include "FrogFunctionLibrary.h"
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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	class UParticleSystemComponent* WhirlwindParticles;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	class UChildActorComponent* WhirlwindPivot;
	UPROPERTY (VisibleAnywhere, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* RightHandCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* LeftHandCollision;
	UPROPERTY()
	FCharacterSettings NeutralModeSettings;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character | PowerMode", meta = (AllowPrivateAccess =
		"true"))
	FCharacterSettings PowerModeSettings;

	FTimerHandle TimerHandle;

public:
	AFrogGameCharacter();


	/** Accessor Function for Current Score */
	UFUNCTION(BlueprintCallable, Category = "Character | Score")
	float GetCurrentScore() const
	{
		return CurrentScore;
	}

	/** Updates the players score 
	* @Param Score This is the amount to increase the players score by. This should only be positive!
	*/
	UFUNCTION(BlueprintCallable, Category = "Character | Score")
	void UpdateCurrentScore(const float Score)
	{
		CurrentScore = CurrentScore + Score;
	}

	UFUNCTION(BlueprintCallable, Category = "Character | PowerMode")
	float GetCurrentPowerPoints() const
	{
		return CurrentPowerPoints;
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

	void Consume(AActor* OtherActor);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | PowerMode")
	bool bPowerMode{false};
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character | PowerMode")
	bool bIsPunching{false};
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


	/** The Players current score */
	UPROPERTY(EditAnywhere, SaveGame, Category = "Character | Score")
	float CurrentScore;
	// How close object has to be to be eaten (destroyed).
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Whirlwind")
	float EatDistance{150.f};
	// How close the object has to be before it starts shrinking
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Whirlwind")
	float ShrinkDistance{500.f};
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
	// End of APawn interface

private:

	void FilterOccludedObjects();
	FVector RotateToAxis(const FVector& A, const FVector& Axis) const;
	/**
	 * Max Radius needs to be a factor of distance ahead of the player.
	 * So if target is at max range from player, then the factor equals 1.
	 * If it's close, then the factor is much smaller.
	 */
	float CalcMaxRadius(AActor* Actor) const;


	void Whirlwind();
	void DoWhirlwind(float DeltaTime);
	void EndWhirlwind();
	bool bUsingWhirlwind{false};

	UPROPERTY()
	FSwirlInfo DefaultWhirlwindSwirl;
	UPROPERTY()
	TArray<AActor*> WhirlwindAffectedActors;
	UPROPERTY()
	TArray<AActor*> PotentialTargets;

	/** Uses fist to punch something, can only be used in power mode **/
	void Punch();
	void OnPunchEnd();
	void RemoveHandCollision() const;
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

	void SaveGame();


	void LoadGame();
	/** Changing to PowerMode **/
	void PowerMode();
	void SetPlayerModel(const FCharacterSettings& CharacterSettings) const;
	void PowerDrain(float DeltaTime);
	void DeactivatePowerMode();

public:

	void Tick(float DeltaTime) override;


	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};
