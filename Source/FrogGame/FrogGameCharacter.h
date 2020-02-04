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

	UPROPERTY(BlueprintReadWrite)
	bool bTongueSpawned{false};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	uint8 SizeTier{1};

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ATongueProjectile> Tongue;

	UPROPERTY()
	class UCableComponent* Cable;

	UPROPERTY()
	AActor* CurrentTarget;

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
	void UpdateCameraBoom(float ScaleDelta);

	/** Uses the tongue to eat something, and then grows **/
	void Lickitung();

	/** Lets the frog jump higher by charging a jump **/
	void StartJump();

	/** Modifier for jump **/
	void ChargeJump(float DeltaTime);

	void ExecuteJump();

	float ScaleAlpha{0.0f};
	bool bScalingUp{false};
	FVector DesiredScale{0};

	float BaseJump{450};
	float JumpBonus{450};
	// The speed at which the jump charges to max velocity when holding down spacebar.
	UPROPERTY(EditAnywhere, Category = Character)
	float ChargeSpeed{1.5f};
	float JumpModifier{0};
	bool bIsCharging{false};

	UFUNCTION()
	void OnBoxTraceEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


public:

	void Tick(float DeltaTime) override;
	void AutoAim();

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};
 
