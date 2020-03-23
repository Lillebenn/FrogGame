// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FrogFunctionLibrary.generated.h"
/**
 * 
 */
USTRUCT(BlueprintType)
struct FSwirlInfo
{
	GENERATED_BODY()

	float RadianDelta{0.f};
	float LinearUpPosition{0.f};
	float CurrentRadius{0.f};
	float MaxRadius;

	float LinearUpSpeed{10.f};
	float LinearInSpeed{10.f};
	float AngularSpeed{10.f};

	void Construct()
	{
		LinearUpSpeed = DefaultLinearUpSpeed;
		LinearInSpeed = DefaultLinearInSpeed;
		AngularSpeed = DefaultAngularSpeed;
	}

	// How far in towards the pivot it can go.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Swirl Settings")
	float MinRadius{10.f};
	// Relative to the player character, starting from the middle. How far up it should go.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Swirl Settings")
	float MaxZPosition{30.f};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Swirl Settings", meta = (DisplayName="Linear Up Speed"))
	float DefaultLinearUpSpeed{10.f};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Swirl Settings", meta = (DisplayName="Linear In Speed"))
	float DefaultLinearInSpeed{10.f};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Swirl Settings", meta = (DisplayName="Angular Speed"))
	float DefaultAngularSpeed{10.f};
};

class FROGGAME_API FrogFunctionLibrary
{
public:
	/**
	 * Calculates the new position of an object following a capsule coordinate system.
	 * Returns true if the current distance from the pivot point is less than the minimum radius set in FSwirlInfo.
	 */
	static bool Swirl(float DeltaTime, FSwirlInfo& SwirlInfo, const FVector& PivotPoint, FVector& OutPosition);
	static bool HorizontalSwirl(float DeltaTime, FSwirlInfo& SwirlInfo, const FVector& PivotPoint,
	                            FVector& OutPosition);
	/**
	 * Finds the squared distance between vector A and B according to a capsule coordinate system. Get the square root for a more accurate value.
	 */
	static float SquaredRadialDistance(const FVector& A, const FVector& B);
private:
	FrogFunctionLibrary() = default;
	static bool UpdateSwirlInfo(float DeltaTime, FSwirlInfo& SwirlInfo);
};
