// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Edible.h"
#include "FFrogLibrary.h"
#include "Sound/SoundCue.h"

#include "AdvCreature.generated.h"
USTRUCT(BlueprintType)
struct FPatrolPath
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FVector> PathPoints;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsLooping{false};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (MakeEditWidget))
	float WaitTime{0.5f};
};

/**
 * Creature with the ability to use animations and more advanced AI.
 */
UCLASS(Abstract)
class FROGGAME_API AAdvCreature : public ACharacter, public IEdible
{
	GENERATED_BODY()


public:
	// Sets default values for this character's properties
	AAdvCreature();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UEdibleComponent* EdibleComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UCustomDestructibleComponent* DestructibleComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	bool bUsePatrolPath{true};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI", meta = (EditCondition = "bUsePatrolPath"))
	FPatrolPath PatrolSettings;

	// Death sound. Not used for frog child.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Sound)
	TArray<USoundCue*> HumanSounds;
	/**
	 * Supposed to be used to get death sounds for humans, but due to some miscommunication, we don't use AdvCreature for humans.
	 */
	UFUNCTION(BlueprintCallable)
	FORCEINLINE USoundCue* GetHumanSound() const
	{
		return FFrogLibrary::GetRandomSoundByArray(HumanSounds);
	}

	// Called every frame
	void Tick(float DeltaTime) override;
	void ActivatePhysics() const;
	float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	                 AActor* DamageCauser) override;
	FTimerHandle TimerHandle;
	// Called to bind functionality to input
	void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	bool IsDisabled_Implementation() override;
	bool bShouldDestroy{false};


	void DisableActor_Implementation() override;
	void PauseAI_Implementation(bool bPause = true) override;
	UFUNCTION(BlueprintImplementableEvent)
	void StopAllActions();
	UFUNCTION(BlueprintImplementableEvent)
	void StartActing();
	UPROPERTY(BlueprintReadWrite)
	bool bIsIdle{true};
	void SetActive_Implementation(bool bActive) override;

protected:
	// Called when the game starts or when spawned
	void BeginPlay() override;
};
