// Fill out your copyright notice in the Description page of Project Settings.


#include "FrogGameInstance.h"
#include "UObject/ConstructorHelpers.h"

UFrogGameInstance::UFrogGameInstance(const FObjectInitializer& ObjectInitialzer)
{
	// Find the widget and assign 
	static ConstructorHelpers::FClassFinder<UUserWidget> InGameUIBPClass(TEXT("/Contenr/Blueprints/FGIngame"));

	if (InGameUIBPClass.Class != nullptr)
	{
		InGameUIClass = InGameUIBPClass.Class;
	}
}

void UFrogGameInstance::LoadIngameUI()
{
	if (InGameUIClass == nullptr) return;

	InGameUI = CreateWidget<UFrogGameUI>(this, InGameUIClass);
	if (InGameUI == nullptr) return;

	InGameUI->AddToViewport();
}
