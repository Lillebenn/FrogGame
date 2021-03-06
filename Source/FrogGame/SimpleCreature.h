// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Edible.h"
#include "FFrogLibrary.h"
#include "Sound/SoundCue.h"

#include "SimpleCreature.generated.h"

UCLASS(Abstract)
class FROGGAME_API ASimpleCreature : public APawn, public IEdible
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* CreatureMesh;

public:
	// Sets default values for this pawn's properties
	ASimpleCreature();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UCapsuleComponent* CapsuleComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UEdibleComponent* EdibleComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UCustomDestructibleComponent* DestructibleComponent;
	UPROPERTY(BlueprintReadOnly)
	class UFloatingPawnMovement* MovementComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Damage)
	TSubclassOf<AActor> Projectile;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Sound)
	TArray<USoundCue*> ShootingSounds;
	UFUNCTION(BlueprintCallable)
	FORCEINLINE USoundCue* GetShootSound() const
	{
		return FFrogLibrary::GetRandomSoundByArray(ShootingSounds);
	}

	// Mostly only relevant for tanks and cars
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Sound)
	USoundBase* AmbientSound;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Sound)
	bool bShouldSqueak{false};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Sound)
	TArray<USoundCue*> SqueakSounds;
	UFUNCTION(BlueprintCallable)
	FORCEINLINE USoundCue* GetSqueakSound() const
	{
		return FFrogLibrary::GetRandomSoundByArray(SqueakSounds);
	}

	UStaticMeshComponent* GetMesh();

	void DisableActor_Implementation() override;
	void PauseAI_Implementation(bool bPause = true) override;
	bool IsDisabled_Implementation() override;
	void SetActive_Implementation(bool bActive) override;
	void PlayHitSound_Implementation() const override;

	void ActivatePhysics() const;
	bool bShouldDestroy{false};

	FTimerHandle TimerHandle;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	int MoveDistance{500};

	float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	                 AActor* DamageCauser) override;

	UFUNCTION(BlueprintImplementableEvent)
	void StopAllActions();
	UFUNCTION(BlueprintImplementableEvent)
	void StartActing();
	UPROPERTY(BlueprintReadWrite)
	bool bIsIdle{true};


protected:
	// Called when the game starts or when spawned
	void BeginPlay() override;
public:
	// Called every frame
	void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
