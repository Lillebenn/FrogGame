// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FrogFunctionLibrary.h"
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
	float MoveToPlayerSpeed{2000.f};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Swirl Settings")
	float InitialRadius{100.f};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Swirl Settings")
	float InitialZPosition{-30.f};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Swirl Settings")
	FSwirlInfo SwirlInfo;

	FTimerHandle SphereLifeSpan;
protected:
	// Called when the game starts or when spawned
	void BeginPlay() override;

	UPROPERTY()
	class AFrogGameCharacter* Frog;
private:
	void MoveToPlayer(float DeltaTime);
	bool bShouldSwirl{false};

	void Consume();
	FVector StartPosition;

public:
	// Called every frame
	void Tick(float DeltaTime) override;
};


