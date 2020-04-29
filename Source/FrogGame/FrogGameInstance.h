// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "FrogGameUI.h"
#include "FrogGameInstance.generated.h"

struct FActorSaveData;
/**
 * 
 */
UCLASS()
class FROGGAME_API UFrogGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	explicit UFrogGameInstance(const FObjectInitializer& ObjectInitializer);


private:

	// Reference to the Grid
	class AGrid* CurrentGrid;


};
