// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "SphereDrop.generated.h"

UCLASS()
class FROGGAME_API ASphereDrop : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* StaticMesh;

public:
	// Sets default values for this actor's properties
	ASphereDrop();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UEdibleComponent* EdibleComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Swirl Settings")
	float InitialRadius{50.f};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Swirl Settings")
	float MinRadius{10.f};
	float CurrentRadius;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Swirl Settings")
	float LinearUpSpeed{10.f};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Swirl Settings")
	float InitialZPosition{-30.f};
	// Relative to the player character, starting from the middle.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Swirl Settings")
	float MaxZPosition{30.f};
	float LinearUpPosition;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Swirl Settings")
	float LinearInSpeed{10.f};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Swirl Settings")
	float AngularSpeed{10.f};
	float RadianDelta{0.f};

protected:
	// Called when the game starts or when spawned
	void BeginPlay() override;
	void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY()
	class AFrogGameCharacter* Frog;
private:
	void MoveToPlayer(float DeltaTime);
	void Swirl(float DeltaTime);
	bool bShouldSwirl{false};
	float FindRadialDistance() const;

	FVector StartPosition;

public:
	// Called every frame
	void Tick(float DeltaTime) override;
};
