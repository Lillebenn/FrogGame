// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EdibleComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FROGGAME_API UEdibleComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UEdibleComponent();

	UPROPERTY(EditAnywhere, Category = Edible)
	bool bAllowSuction{false};
	// Amount of Points earned towards getting powermode gained when eating this object.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Edible)
	float PowerPoints{0.1f};
	// Amount of Score granted for eating this edible.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Edible)
	float ScorePoints{500.f};
	UPROPERTY()
	AActor* Parent{nullptr};
	float AngleAxis{2.f};
	float PivotDistance;
	bool bConsumed{false};
	void IgnorePawnCollision() const;
protected:
	// Called when the game starts
	void BeginPlay() override;

public:
	// Called every frame
	void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
