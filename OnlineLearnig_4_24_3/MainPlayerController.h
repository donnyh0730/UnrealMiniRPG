// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "MainPlayerController.generated.h"

class UUserWidget;
class AEnemy;
class UEnemyHealthBar;
/**
 * 
 */
UCLASS()
class ONLINELEARNIG_4_24_3_API AMainPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	/* Reference to the UMG asset in the editor */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<UUserWidget> HudOverlayAsset;

	/*Variable to hold the widget after creating it */
	UPROPERTY(BlueprintReadWrite, Category = "Widgets")
	UUserWidget* HUDOverlay;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<UUserWidget> WEnemyHealthBar;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Widgets")
	TArray<UEnemyHealthBar*> EnemyHealthBars;

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

public:

	void AttachWidgettoNewEnemy(AEnemy* enemy);

	void DeleteWidgetFromEnemy(AEnemy* enemy);
};
