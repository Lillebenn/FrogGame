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

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	/** Static mesh that is used to spawn linetraces **/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* RayMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* BoxCollider;
public:
	AFrogGameCharacter();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	class UStaticMeshComponent* GetRayMesh();
	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	/** The length of the frogs body, used to calculate tongue length and ray-casting area. **/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
	float FrogLength;


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bTongueSpawned{false};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	uint8 SizeTier{1};

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ATongueProjectile> Tongue;

	UPROPERTY()
	class UCableComponent* Cable;

	void Consume(AActor* OtherActor);

	UPROPERTY(VisibleAnywhere, Category = Character)
	FVector ScaledCapsuleSize;
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
	void UpdateCameraBoom(const float ScaleDelta);

	/** Uses the tongue to eat something, and then grows **/
	void Lickitung();

	/** Lets the frog jump higher by charging a jump **/
	void JumpCharge();

	/** Modifier for jump **/
	void Jumptick(float DeltaTime);

	void FroggyJump();

	float ScaleLerp{0.0f};
	bool Lerping{false};
	FVector DesiredScale{0};
	// float JumpZVelocity{};
	float BaseJump{450};
	float JumpBonus{ 450 };
	float DefaultVelocity{};
	float JumpModifier = FMath::Clamp<float>(JumpModifier, 0.f, 1.f);
	bool IsHolding{ false };


	/** Handles the charging of JumpCharge **/
	FTimerHandle JumpChargeTimerHandle;

public:

	void Tick(float DeltaTime) override;
	void AutoAim();

	void BeginPlay();

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};
