// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FFrogLibrary.h"
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
	float InitialRadius{140.f};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Swirl Settings")
	float InitialZPosition{-40.f};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Swirl Settings")
	FSwirlInfo SwirlInfo;

	FTimerHandle SphereLifeSpan;

	void Activate();
	void Deactivate();
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


