// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AdvCreature.h"
#include "StaticAdvCreature.generated.h"

/**
 * 
 */
UCLASS()
class FROGGAME_API AStaticAdvCreature : public AAdvCreature
{
	GENERATED_BODY()
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* CreatureMesh;
public:
	AStaticAdvCreature();
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


	UStaticMeshComponent* GetStaticMesh();

	void DisableActor_Implementation() override;
	void PauseAI_Implementation(bool bPause = true) override;
	bool IsDisabled_Implementation() override;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	int MoveDistance{500};
};
