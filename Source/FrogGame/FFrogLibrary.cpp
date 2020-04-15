// Fill out your copyright notice in the Description page of Project Settings.


#include "FFrogLibrary.h"


#include "HAL/PlatformFilemanager.h"
#include "Misc/LocalTimestampDirectoryVisitor.h"
#include "Misc/Paths.h"

bool FFrogLibrary::Swirl(const float DeltaTime, FSwirlInfo& SwirlInfo, const FVector& PivotPoint,
                         FVector& OutPosition)
{
	const bool bMinRadius{UpdateSwirlInfo(DeltaTime, SwirlInfo)};

	OutPosition.X = PivotPoint.X + SwirlInfo.CurrentRadius * FMath::Cos(SwirlInfo.RadianDelta);
	OutPosition.Y = PivotPoint.Y + SwirlInfo.CurrentRadius * FMath::Sin(SwirlInfo.RadianDelta);
	OutPosition.Z = PivotPoint.Z + SwirlInfo.LinearUpPosition;
	return bMinRadius;
}

bool FFrogLibrary::HorizontalSwirl(const float DeltaTime, FSwirlInfo& SwirlInfo, const FVector& PivotPoint,
                                   FVector& OutPosition)
{
	const bool bMinRadius{UpdateSwirlInfo(DeltaTime, SwirlInfo)};

	OutPosition.X = PivotPoint.X + SwirlInfo.CurrentRadius * FMath::Sin(SwirlInfo.RadianDelta);
	OutPosition.Y = PivotPoint.Y + SwirlInfo.LinearUpPosition;
	OutPosition.Z = PivotPoint.Z + SwirlInfo.CurrentRadius * FMath::Cos(SwirlInfo.RadianDelta);
	return bMinRadius;
}

bool FFrogLibrary::UpdateSwirlInfo(const float DeltaTime, FSwirlInfo& SwirlInfo)
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

float FFrogLibrary::SquaredRadialDistance(const FVector& A, const FVector& B)
{
	const FVector2D FrogXY{A - B};
	const float SquaredRadialDistance{(FrogXY.X * FrogXY.X) + (FrogXY.Y * FrogXY.Y)};
	return SquaredRadialDistance;
}

TArray<FString> FFrogLibrary::GetAllFilesInDirectory(FString directory, bool fullPath,
                                                     FString onlyFilesStartingWith, FString onlyFilesEndingWith)
{
	// Get all files in directory
	TArray<FString> directoriesToSkip;
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	FLocalTimestampDirectoryVisitor Visitor(PlatformFile, directoriesToSkip, directoriesToSkip, false);
	PlatformFile.IterateDirectory(*directory, Visitor);
	TArray<FString> files;

	for (TMap<FString, FDateTime>::TIterator TimestampIt(Visitor.FileTimes); TimestampIt; ++TimestampIt)
	{
		const FString filePath = TimestampIt.Key().LeftChop(7);
		const FString fileName = FPaths::GetCleanFilename(filePath);
		bool shouldAddFile = true;

		// Check if filename starts with required characters
		if (!onlyFilesStartingWith.IsEmpty())
		{
			const FString left = fileName.Left(onlyFilesStartingWith.Len());

			if (!(fileName.Left(onlyFilesStartingWith.Len()).Equals(onlyFilesStartingWith)))
				shouldAddFile = false;
		}

		// Check if file extension is required characters
		if (!onlyFilesEndingWith.IsEmpty())
		{
			if (!(FPaths::GetExtension(fileName, false).Equals(onlyFilesEndingWith, ESearchCase::IgnoreCase)))
				shouldAddFile = false;
		}

		// Add full path to results
		if (shouldAddFile)
		{
			UE_LOG(LogTemp, Warning, TEXT("%s"), *filePath);

			files.Add(fullPath ? filePath : fileName);
		}
	}

	return files;
}
