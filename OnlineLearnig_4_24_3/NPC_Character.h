// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include <Camera/CameraComponent.h>
#include "NPC_Character.generated.h"


class AAIController;
class USphereComponent;
class USoundCue;
class AMain;
UCLASS()
class ONLINELEARNIG_4_24_3_API ANPC_Character : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ANPC_Character();

	UPROPERTY(EditAnywhere)
	UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	USphereComponent* RecognitionSphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC")
	int32 MeassageCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC")
	int32 MaxMessageCount;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NPC")
	bool b_NearCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NPC")
	AMain* MainCharacter;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	UFUNCTION()
	virtual void RecognitionSphereOnOverlapbegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	UFUNCTION()
	virtual void RecognitionSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	void OnInteractInBase();

	UFUNCTION(BlueprintImplementableEvent)
	void OnInteract();

};
