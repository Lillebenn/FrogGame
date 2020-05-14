// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpatialPartition.generated.h"

UCLASS()
class FROGGAME_API ASpatialPartition : public AActor
{
	GENERATED_BODY()


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* DisableVolume;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* EnableVolume;
public:
	// Sets default values for this actor's properties
	ASpatialPartition();
	void InitDisableActors();

	UFUNCTION()
	void OnPlayerOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	               int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnPlayerEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                  int32 OtherBodyIndex);

	// Whether we should disable objects inside the DisableVolume at start. Turn off to leave objects inside it enabled until the player leaves it for the first time.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = DisableVolume)
	bool bDisableAtStart{true};
	FTimerHandle Test;
	

protected:
	// Called when the game starts or when spawned
	void BeginPlay() override;

private:
	UPROPERTY()
	TArray<AActor*> DisabledActors;
};
