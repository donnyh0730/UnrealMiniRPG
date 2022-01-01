// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FloorSwitch.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class AMain;
UCLASS()
class ONLINELEARNIG_4_24_3_API AFloorSwitch : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFloorSwitch();

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category = "Floor Switch")
	UBoxComponent* TriggerBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Floor Switch")
	UStaticMeshComponent* FloorSwitch;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Floor Switch")
	UStaticMeshComponent* Door;

	UPROPERTY(BlueprintReadWrite,Category = "Floor Switch")
	FVector DoorInitiallocation;

	UPROPERTY(BlueprintReadWrite, Category = "Floor Switch")
	FVector SwitchInitiallocation;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite, Category = "Floor Switch")
	AMain* Main;

private:
	
	FTimerHandle LowerDoorTimerHandler;

	bool bIsOnPawn;

	bool bRaising;
	bool bLowering;

	float MovingDistance;

	float CurrentMoveDistance;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable,Category ="Floor Switch")
	void RaiseDoor();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable,Category = "Floor Switch")
	void LowerDoor();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Floor Switch")
	void RaiseFloorSwitch();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Floor Switch")
	void LowerFloorSwitch();
	
	UFUNCTION(BlueprintCallable, Category = "Floor Switch")
	void UpdateDoorLocation(float Z);
	
	UFUNCTION(BlueprintCallable, Category = "Floor Switch")
	void UpdateFloorSwitchLocation(float Z);

	UFUNCTION(BlueprintCallable)
	void LowerDoorInSeconds(float sec);

	void CloseDoor();
	
	//UFUNCTION()
	//void AnimatingStuff();

};
