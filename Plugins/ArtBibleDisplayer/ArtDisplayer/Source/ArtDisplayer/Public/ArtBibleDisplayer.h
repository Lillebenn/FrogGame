// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ArtBibleDisplayer.generated.h"

UCLASS()
class ARTDISPLAYER_API AArtBibleDisplayer : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AArtBibleDisplayer();

protected:
	// Called when the game starts or when spawned
	void BeginPlay() override;

public:
	// Called every frame
	void Tick(float DeltaTime) override;

	UPROPERTY()
	class UStaticMeshComponent* DisplayedObject;
	// Add static meshes to this array in the order you want them displayed.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	TArray<UStaticMesh*> StaticMeshes;
	// Folders to fill from if desired. Leave empty to only add assets manually. Not guaranteed to find all meshes.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera)
	TArray<FString> AutoFillFolders;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	// Should the Art Displayer automatically switch to the next mesh in line?
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera)
	bool bAutoDisplay{true};
	// Should the Art Displayer loop back around after reaching the end of the array?
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera)
	bool bLoop{true};
	// Should the camera rotate around the object?
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera)
	bool bRotate{true};
	// Set to true if we should attempt to do a full rotation around the object before we switch to a new mesh. Overrides the delay timer.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera)
	bool bFullRotation{false};
	// How quickly the rotation should happen.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera)
	float RotationSpeed{50.f};
	// Time between mesh switches.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (DisplayName =
		"Delay", EditCondition = "!bFullRotation"))
	float DelayTime{2.f};
	// Index to display next (or first if set in defaults.) 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (EditCondition = "!bAutoDisplay"))
	int NextIndex{0};
	// Index of current asset shown. 
	UPROPERTY(VisibleInstanceOnly, Category = Camera)
	int Index;
	// Should the camera boom offset its Z position to account for a low pivot point? (Try to set the pivot points of your meshes to the middle of the mesh if possible for optimal viewing.)
	UPROPERTY(EditAnywhere, Category = Camera)
	bool bCameraZOffset{false};

private:

	float CurrentDelay;
	float CurYaw{0.f};

	void SwitchDisplayedObject();

	void MeshArrayFromList();
};
