// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMesh.h"
#include "Engine/SkeletalMesh.h"


/**
 * 
 */
class ARTDISPLAYER_API FileLoader
{
public:
	/*
	* @param Directory Absolute path of the directory to search.
	* @param bFullPath Whether to return the full path of each object or just the filename.
	* @param bStripExtension Whether to strip the extension from the file name.
	*/
	static TArray<FString> GetAllFilesInDirectory(FString Directory,
	                                              bool bFullPath = true,
	                                              bool bStripExtension = true,
	                                              FString OnlyFilesStartingWith = TEXT(""),
	                                              FString OnlyFilesEndingWith = TEXT(""));

	static FORCEINLINE FName GetObjPath(const UObject* Obj)
	{
		if (!Obj) return NAME_None;
		if (!Obj->IsValidLowLevel()) return NAME_None;
		//~

		const FStringAssetReference ThePath = FStringAssetReference(Obj);

		if (!ThePath.IsValid()) return NAME_None;

		//The Class FString Name For This Object
		FString Str = Obj->GetClass()->GetDescription();

		Str += "'";
		Str += ThePath.ToString();
		Str += "'";

		return FName(*Str);
	}

	//TEMPLATE Load Obj From Path
	template <typename ObjClass>
	static FORCEINLINE ObjClass* LoadObjFromPath(const FName& Path)
	{
		if (Path == NAME_None) return nullptr;
		//~

		return Cast<ObjClass>(StaticLoadObject(ObjClass::StaticClass(), NULL, *Path.ToString()));
	}

	static FORCEINLINE UStaticMesh* LoadStaticMeshFromPath(const FName& Path)
	{
		if (Path == NAME_None) return nullptr;
		//~

		return LoadObjFromPath<UStaticMesh>(Path);
	}

	static FORCEINLINE USkeletalMesh* LoadSkeletalMeshFromPath(const FName& Path)
	{
		if (Path == NAME_None) return nullptr;
		//~

		return LoadObjFromPath<USkeletalMesh>(Path);
	}

private:
	FileLoader() = default;
	~FileLoader() = default;
};
