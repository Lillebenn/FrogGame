// Fill out your copyright notice in the Description page of Project Settings.


#include "FrogFunctionLibrary.h"

bool FrogFunctionLibrary::Swirl(const float DeltaTime, FSwirlInfo& SwirlInfo, const FVector& PivotPoint,
                                FVector& OutPosition)
{
	const bool bMinRadius{UpdateSwirlInfo(DeltaTime, SwirlInfo)};

	OutPosition.X = PivotPoint.X + SwirlInfo.CurrentRadius * FMath::Cos(SwirlInfo.RadianDelta);
	OutPosition.Y = PivotPoint.Y + SwirlInfo.CurrentRadius * FMath::Sin(SwirlInfo.RadianDelta);
	OutPosition.Z = PivotPoint.Z + SwirlInfo.LinearUpPosition;
	return bMinRadius;
}

bool FrogFunctionLibrary::HorizontalSwirl(const float DeltaTime, FSwirlInfo& SwirlInfo, const FVector& PivotPoint,
                                          FVector& OutPosition)
{
	const bool bMinRadius{UpdateSwirlInfo(DeltaTime, SwirlInfo)};

	OutPosition.X = PivotPoint.X + SwirlInfo.CurrentRadius * FMath::Sin(SwirlInfo.RadianDelta);
	OutPosition.Y = PivotPoint.Y + SwirlInfo.LinearUpPosition;
	OutPosition.Z = PivotPoint.Z + SwirlInfo.CurrentRadius * FMath::Cos(SwirlInfo.RadianDelta);
	return bMinRadius;
}

bool FrogFunctionLibrary::UpdateSwirlInfo(const float DeltaTime, FSwirlInfo& SwirlInfo)
{
	bool bMinRadius{false};
	if (SwirlInfo.CurrentRadius <= SwirlInfo.MinRadius)
	{
		bMinRadius = true;
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
	return bMinRadius;
}

float FrogFunctionLibrary::SquaredRadialDistance(const FVector& A, const FVector& B)
{
	const FVector2D FrogXY{A - B};
	const float SquaredRadialDistance{(FrogXY.X * FrogXY.X) + (FrogXY.Y * FrogXY.Y)};
	return SquaredRadialDistance;
}
