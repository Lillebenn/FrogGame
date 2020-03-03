// Fill out your copyright notice in the Description page of Project Settings.


#include "Edible.h"

int IEdible::CalculateSizeTier(const float InSize, const int NumTiers)
{
	// This will 100% need to be changed later. Just a temporary fix for automatically giving out size tiers.
	// In the future, size tiers will likely be separated by more than 10.f as the 
	float MinVal{0.f}, MaxVal{10.f};
	for (int i{0}; i < NumTiers; i++)
	{
		if (FMath::IsWithin(InSize, MinVal, MaxVal))
		{
			return i;
		}
		MinVal = MaxVal;
		MaxVal *= 2.5f; // This is probably too exponential.
	}
	return 10; // 10 is current max tier I guess.
}

// Add default functionality here for any IEdible functions that are not pure virtual.
FTransform IEdible::GetStartTransform()
{
	return FTransform();
}
