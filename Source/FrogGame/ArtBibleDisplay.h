// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ArtBibleDisplay.generated.h"

UCLASS()
class FROGGAME_API AArtBibleDisplay : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AArtBibleDisplay();

protected:
	// Called when the game starts or when spawned
	void BeginPlay() override;

public:
	// Called every frame
	void Tick(float DeltaTime) override;

	UPROPERTY()
	class UStaticMeshComponent* DisplayedObject;
	UPROPERTY(EditAnywhere, Category = Camera)
	class UCineCameraComponent* CineCam;
	// Add static meshes to this array in the order you want them displayed.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	TArray<UStaticMesh*> StaticMeshes;
	// Folders to fill from if desired. Leave empty to only add assets manually. 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera)
	TArray<FString> AutoFillFolders;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	// Whether the displayer should automatically switch between meshes.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera)
	bool bAutoDisplay{true};
	// Whether the displayer should loop back around after reaching the end of the mesh array.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera)
	bool bShouldLoop{true};
	// Whether the camera should rotate around the object.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera)
	bool bShouldRotate{true};
	// Set to true if we should attempt to do a full rotation around the object before we switch to a new mesh. Overrides the delay timer.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera)
	bool bFullRotation{false};
	// How quickly the rotation should happen.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera)
	float RotationSpeed{50.f};
	// Time between mesh switches.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (DisplayName =
		"Delay"))
	float DelayTime{2.f};
	// Index to display next (or first if set in defaults.) 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera)
	int NextIndex{0};
	// Index of current asset shown. 
	UPROPERTY(VisibleInstanceOnly, Category = Camera)
	int Index;
	// How far out the boom should go depending on the size of the item. This probably won't mean much to you.
	UPROPERTY(EditAnywhere, Category = Camera)
	float DistanceMultiplier{5.5f};

private:

	float CurrentDelay;
	float CurYaw{0.f};

	void SwitchDisplayedObject();

	void MeshArrayFromList();
};
