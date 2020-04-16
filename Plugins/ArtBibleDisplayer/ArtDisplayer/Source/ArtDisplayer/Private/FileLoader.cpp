// Fill out your copyright notice in the Description page of Project Settings.


#include "FileLoader.h"
#include "Misc/LocalTimestampDirectoryVisitor.h"
#include "HAL/PlatformFilemanager.h"
#include "Misc/Paths.h"

TArray<FString> FileLoader::GetAllFilesInDirectory(FString Directory, bool bFullPath,
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
