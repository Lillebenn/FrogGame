// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "FrogGameMode.h"
#include "FrogGameCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "FrogGameInstance.h"
#include "SphereDrop.h"


UFrogGameUI* AFrogGameMode::ConstructWidget(const TSubclassOf<UFrogGameUI> Subclass) const
{
	if (Subclass)
	{
		UFrogGameUI* NewWidget{CreateWidget<UFrogGameUI>(GetWorld(), Subclass)};

		if (NewWidget)
		{
			NewWidget->AddToViewport();
			return NewWidget;
		}
	}
	return nullptr;
}


void AFrogGameMode::BeginPlay()
{
	Super::BeginPlay();
	if (SphereType)
	{
		for (int i{0}; i < MaxNumSphere; i++)
		{
			const FVector Scale{0.5f, 0.5f, 0.5f};
			const FTransform SpawnTransform{FRotator(), FVector(), Scale};
			Spheres.Add(GetWorld()->SpawnActor<ASphereDrop>(SphereType, SpawnTransform));
		}
	}
}



ASphereDrop* AFrogGameMode::SpawnAvailableSphere(const FVector SpawnPosition)
{
	for (auto Sphere : Spheres)
	{
		if (Sphere->IsHidden())
		{
			Sphere->SetActorLocation(SpawnPosition);
			Sphere->Activate();
			return Sphere;
		}
	}
	return nullptr;
}


void AFrogGameMode::SetVisibleWidget(const TEnumAsByte<ECurrentWidget> NewVisibleWidget)
{
	if (!bIsBeginPlay)
	{
		switch (CurrentWidget)
		{
		case ECurrentWidget::InGameHUD:
			FrogHUDWidget->PlayOutAnimation();
			break;
		case ECurrentWidget::MainMenu:
			MainMenuWidget->PlayOutAnimation();
			break;
		case ECurrentWidget::PauseMenu:
			PauseMenuWidget->PlayOutAnimation();
			break;
		case ECurrentWidget::EndScreen:
			EndScreenWidget->PlayOutAnimation();
		}
	}
	else
	{
		bIsBeginPlay = false;
	}
	switch (NewVisibleWidget)
	{
	case ECurrentWidget::InGameHUD:
		if (!FrogHUDWidget)
		{
			FrogHUDWidget = ConstructWidget(FrogHUDClass);
		}
		FrogHUDWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		FrogHUDWidget->PlayInAnimation();
		break;
	case ECurrentWidget::MainMenu:
		if (!MainMenuWidget)
		{
			MainMenuWidget = ConstructWidget(MainMenuClass);
		}
		MainMenuWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		MainMenuWidget->PlayInAnimation();
		OpenedMainMenu();
		break;
	case ECurrentWidget::PauseMenu:
		if (!PauseMenuWidget)
		{
			PauseMenuWidget = ConstructWidget(PauseMenuClass);
		}
		PauseMenuWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		PauseMenuWidget->PlayInAnimation();
		break;
	case ECurrentWidget::EndScreen:
		if (!EndScreenWidget)
		{
			EndScreenWidget = ConstructWidget(EndScreenClass);
		}
		EndScreenWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		EndScreenWidget->PlayInAnimation();
		OpenedEndScreen();
	default:
		break;
	}
	CurrentWidget = NewVisibleWidget;
}

UFrogGameUI* AFrogGameMode::GetInGameHUD()
{
	if (!FrogHUDWidget)
	{
		FrogHUDWidget = ConstructWidget(FrogHUDClass);
	}
	return FrogHUDWidget;
}

void AFrogGameMode::SetCurrentWidgetVisibility()
{
	switch (CurrentWidget)
	{
	case ECurrentWidget::InGameHUD:
		FrogHUDWidget->SetVisibility(GetNewVisibility(FrogHUDWidget));
		break;
	case ECurrentWidget::MainMenu:
		MainMenuWidget->SetVisibility(GetNewVisibility(MainMenuWidget));
		break;
	case ECurrentWidget::PauseMenu:
		PauseMenuWidget->SetVisibility(GetNewVisibility(PauseMenuWidget));
		break;
	case ECurrentWidget::EndScreen:
		EndScreenWidget->SetVisibility(GetNewVisibility(EndScreenWidget));
	}
}


ESlateVisibility AFrogGameMode::GetNewVisibility(UFrogGameUI* WidgetToSet)
{
	const ESlateVisibility CurrentVisibility{WidgetToSet->GetVisibility()};
	if (CurrentVisibility != ESlateVisibility::Hidden)
	{
		return ESlateVisibility::Hidden;
	}

	return ESlateVisibility::SelfHitTestInvisible;
}
