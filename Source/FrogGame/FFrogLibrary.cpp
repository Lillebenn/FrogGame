// Fill out your copyright notice in the Description page of Project Settings.


#include "FFrogLibrary.h"


#include "HAL/PlatformFilemanager.h"
#include "Kismet/KismetSystemLibrary.h"
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

TArray<FString> FFrogLibrary::GetAllFilesInDirectory(FString Directory, bool bFullPath,
                                                     bool bStripExtension, FString OnlyFilesStartingWith,
                                                     FString OnlyFilesEndingWith)
{
	// Get all files in directory
	TArray<FString> DirectoriesToSkip;
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	FLocalTimestampDirectoryVisitor Visitor(PlatformFile, DirectoriesToSkip, DirectoriesToSkip, false);
	PlatformFile.IterateDirectory(*Directory, Visitor);
	TArray<FString> Files;
	FString Content{TEXT("/Content")};
	for (TMap<FString, FDateTime>::TIterator TimestampIt(Visitor.FileTimes); TimestampIt; ++TimestampIt)
	{
		FString FilePath = TimestampIt.Key();
		FString FileName;

		if (bStripExtension)
		{
			// Get the filename and path, stripping out the extension
			FileName = FPaths::GetCleanFilename(FilePath);
			int ExtensionLength{FPaths::GetExtension(FileName).Len() + 1};
			FilePath = FilePath.LeftChop(ExtensionLength);
			FileName = FileName.LeftChop(ExtensionLength);
		}
		else
		{
			FileName = FPaths::GetCleanFilename(FilePath);
		}
		// Edit file path to correspond to Content Browser path
		FilePath = TEXT("/Game") + FilePath.RightChop(FilePath.Find(Content) + Content.Len());


		bool bShouldAddFile = true;

		// Check if filename starts with required characters
		if (!OnlyFilesStartingWith.IsEmpty())
		{
			const FString Left = FileName.Left(OnlyFilesStartingWith.Len());

			if (!(FileName.Left(OnlyFilesStartingWith.Len()).Equals(OnlyFilesStartingWith)))
				bShouldAddFile = false;
		}

		// Check if file extension is required characters
		if (!OnlyFilesEndingWith.IsEmpty())
		{
			if (!(FPaths::GetExtension(FileName, false).Equals(OnlyFilesEndingWith, ESearchCase::IgnoreCase)))
				bShouldAddFile = false;
		}

		// Add full path to results
		if (bShouldAddFile)
		{
			Files.Add(bFullPath ? FilePath : FileName);
		}
	}

	return Files;
}
