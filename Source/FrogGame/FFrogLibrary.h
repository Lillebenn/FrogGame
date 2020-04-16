// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMesh.h"
#include "FFrogLibrary.generated.h"
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

class FROGGAME_API FFrogLibrary
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

	static FORCEINLINE UStaticMesh* LoadMeshFromPath(const FName& Path)
	{
		if (Path == NAME_None) return nullptr;
		//~

		return LoadObjFromPath<UStaticMesh>(Path);
	}

private:
	FFrogLibrary() = default;
	static bool UpdateSwirlInfo(float DeltaTime, FSwirlInfo& SwirlInfo);
};
