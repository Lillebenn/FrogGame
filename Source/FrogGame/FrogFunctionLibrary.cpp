// Fill out your copyright notice in the Description page of Project Settings.


#include "FrogFunctionLibrary.h"

bool FrogFunctionLibrary::Swirl(const float DeltaTime, FSwirlInfo& SwirlInfo, const FVector& PivotPoint,
                                FVector& OutPosition)
{
	bool Success{false};
	if (SwirlInfo.CurrentRadius <= SwirlInfo.MinRadius)
	{
		Success = true;
	}
	if (SwirlInfo.RadianDelta >= 2.f * PI)
	{
		SwirlInfo.RadianDelta = 0.f;
	}

	SwirlInfo.RadianDelta += SwirlInfo.AngularSpeed * DeltaTime;
	// Prevent radius from becoming negative (would start reversing direction)
	if (SwirlInfo.CurrentRadius >= 0.f)
	{
		SwirlInfo.CurrentRadius -= SwirlInfo.LinearInSpeed * DeltaTime;
	}

	if (SwirlInfo.LinearUpPosition <= SwirlInfo.MaxZPosition)
	{
		SwirlInfo.LinearUpPosition += SwirlInfo.LinearUpSpeed * DeltaTime;
	}
	OutPosition.X = PivotPoint.X + SwirlInfo.CurrentRadius * FMath::Cos(SwirlInfo.RadianDelta);
	OutPosition.Y = PivotPoint.Y + SwirlInfo.CurrentRadius * FMath::Sin(SwirlInfo.RadianDelta);
	OutPosition.Z = PivotPoint.Z + SwirlInfo.LinearUpPosition;
	return Success;
}

float FrogFunctionLibrary::SquaredRadialDistance(const FVector& A, const FVector& B)
{
	const FVector2D FrogXY{A - B};
	const float SquaredRadialDistance{(FrogXY.X * FrogXY.X) + (FrogXY.Y * FrogXY.Y)};
	return SquaredRadialDistance;
}
