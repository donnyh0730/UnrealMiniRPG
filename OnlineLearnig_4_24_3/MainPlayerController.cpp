// Fill out your copyright notice in the Description page of Project Settings.


#include "MainPlayerController.h"
#include "Enemy.h"
#include "EnemyHealthBar.h"
#include "Blueprint/UserWidget.h"


void AMainPlayerController::BeginPlay()
{
	Super::BeginPlay();
	if (HudOverlayAsset)
	{
		HUDOverlay = CreateWidget<UUserWidget>(this,HudOverlayAsset);
	}
	
	HUDOverlay->AddToViewport();
	HUDOverlay->SetVisibility(ESlateVisibility::Visible);

}

void AMainPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMainPlayerController::AttachWidgettoNewEnemy(AEnemy* enemy)
{
	if (WEnemyHealthBar)
	{
		UEnemyHealthBar* EnemyHealthBar = CreateWidget<UEnemyHealthBar>(this, WEnemyHealthBar);
		EnemyHealthBars.Add(EnemyHealthBar);
		if (EnemyHealthBar)
		{
			enemy->EnemyHealthBar = EnemyHealthBar;
			EnemyHealthBar->AddToViewport();
			EnemyHealthBar->SetVisibility(ESlateVisibility::Visible);
		}
		FVector2D Alignment(0.f, 0.f);
		EnemyHealthBar->SetAlignmentInViewport(Alignment);
	}
}

void AMainPlayerController::DeleteWidgetFromEnemy(AEnemy* enemy)
{
	if (enemy->EnemyHealthBar)
	{
		UEnemyHealthBar* EnemyHealthBar = enemy->EnemyHealthBar;
		EnemyHealthBars.Remove(EnemyHealthBar);
		EnemyHealthBar->RemoveFromParent();
	}
}

