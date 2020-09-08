// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/SphereComponent.h"
#include "Collider.generated.h"

UCLASS()
class ONLINELEARNIG_4_24_3_API ACollider : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACollider();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, Category = "Mesh")
	UStaticMeshComponent* MeshComponent;
	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	USphereComponent* SphereComponent;
	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	class UCameraComponent* Camera;
	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	class USpringArmComponent* SpringArm;
	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	class UColliderMovementComponent* MyMovementComponent;

	virtual UPawnMovementComponent* GetMovementComponent() const override;

	UStaticMeshComponent* GetMeshComponent();
	void SetMeshComponent(UStaticMeshComponent* Mesh);

	FORCEINLINE USphereComponent* GetSphereComponent() { return SphereComponent; }
	FORCEINLINE void SetSphereComponent(USphereComponent* Sphere) { SphereComponent = Sphere; }

	FORCEINLINE UCameraComponent* GetCameraComponent() { return Camera; }
	FORCEINLINE void SetCameraComponent(UCameraComponent* camera) { Camera = camera; }

	FORCEINLINE USpringArmComponent* GetSpringArmComponent() { return SpringArm; }
	FORCEINLINE void SetSpringArmComponent(USpringArmComponent* spring) { SpringArm = spring; }

private:

	void MoveForward(float value);

	void MoveRight(float value);

	void CameraPitch(float value);

	void CameraYaw(float value);
	FVector2D CameraInput;

	FVector CurrentVelocity;
};
