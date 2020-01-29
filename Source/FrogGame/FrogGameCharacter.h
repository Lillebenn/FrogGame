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

public:
	AFrogGameCharacter();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	/** The length of the frogs body, used to calculate tongue length and ray-casting area. **/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
	float FrogLength;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bTongueSpawned{false};

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ATongueProjectile> Tongue;

	UPROPERTY()
	class UCableComponent* Cable;

	void Consume(AActor* OtherActor);
protected:

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

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

protected:
	// APawn interface
	void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

private:

	/** Creates a Raycast infront of the frog that applies a reticle to show what is edible **/
	void AutoAim();

	/** Updates the frogs length **/
	void UpdateLength();

	/** Uses the tongue to eat something, and then grows **/
	void Lickitung();

	float ScaleLerp{0.0f};
	bool Lerping{false};
	FVector DesiredScale{0};


public:

	virtual void Tick(float DeltaTime) override;

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	
	FVector GetEnd();

	FVector GetTarget();
};
