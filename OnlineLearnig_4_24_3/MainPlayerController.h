// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"
#include "MainPlayerController.generated.h"

class UUserWidget;
class AEnemy;
class UEnemyHealthBar;
class UInfoWidget;
class UDialogueWidget;
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<UUserWidget> InfoWidgetAsset;

	UPROPERTY(BlueprintReadWrite, Category = "Widgets")
	UInfoWidget* InfoWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<UUserWidget> DialogueWidgetAsset;

	UPROPERTY(BlueprintReadWrite, Category = "Widgets")
	UDialogueWidget* DialogueWidget;
	
protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

public:

	void AttachWidgettoNewEnemy(AEnemy* enemy);

	void DeleteWidgetFromEnemy(AEnemy* enemy);

	UFUNCTION(BlueprintCallable)
	void SetVisibleInfoWidget(ESlateVisibility visibility);

	UFUNCTION(BlueprintCallable)
	void SetInfoWidgetText(int32 num);

	UFUNCTION(BlueprintCallable)
	void SetInfoWidgetTextByString(FName message);


	UFUNCTION(BlueprintCallable)
	void SetVisibleDialogueWidget(ESlateVisibility visibility);

	UFUNCTION(BlueprintCallable)
	void SetDialogueWidgetText(int32 num);
	
	
};
