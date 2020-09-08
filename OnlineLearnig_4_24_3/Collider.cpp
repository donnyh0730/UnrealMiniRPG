// Fill out your copyright notice in the Description page of Project Settings.


#include "Collider.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/InputComponent.h"
#include "Camera/CameraComponent.h"
#include "ColliderMovementComponent.h"
#include "EngineGlobals.h"
#include "Engine/Engine.h"
#include "GameFrameWork/SpringArmComponent.h"

#define printFString(text, fstring) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Magenta, FString::Printf(TEXT(text), fstring))

// Sets default values
ACollider::ACollider()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->SetupAttachment(GetRootComponent());
	SphereComponent->InitSphereRadius(40.0f);
	SphereComponent->SetCollisionProfileName(TEXT("Pawn"));

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(GetRootComponent());

	//static ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMeshAsset(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere'"));

	//MeshComponent->SetStaticMesh(StaticMeshAsset.Object);
	MeshComponent->SetRelativeLocation(FVector(0, 0, -40));
	MeshComponent->SetWorldScale3D(FVector(0.8f, 0.8f, 0.8f));
	
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->RelativeRotation = FRotator(-45, 0, 0);
	SpringArm->TargetArmLength = 400;
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 3;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm,USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;
	MyMovementComponent = CreateDefaultSubobject<UColliderMovementComponent>(TEXT("MovementComponent"));
	MyMovementComponent->UpdatedComponent = RootComponent;

	CameraInput = FVector2D(0, 0);

	AutoPossessPlayer = EAutoReceiveInput::Player0;

}

// Called when the game starts or when spawned
void ACollider::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACollider::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	//FRotator NewRotation = GetActorRotation();
	//NewRotation.Yaw += CameraInput.X;
	
	//this->AddActorLocalRotation(FRotator(0.f, 0.f, CameraInput.X));

	//FRotator NewSpringArmRotation = SpringArm->GetComponentRotation();
	//NewSpringArmRotation.Pitch += CameraInput.Y;
	//FMath::Clamp(NewSpringArmRotation.Pitch, -80.f, -15.f);
	//NewSpringArmRotation.Yaw = 0.f;
	//NewSpringArmRotation.Roll = 0.f;

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Magenta, FString::SanitizeFloat(CameraInput.Y));
	//SpringArm->AddLocalRotation(FQuat(0,1,0,CameraInput.Y/360));
	
}

// Called to bind functionality to input
void ACollider::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ACollider::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &ACollider::MoveRight);
	
	PlayerInputComponent->BindAxis(TEXT("CameraPitch"), this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis(TEXT("CameraYaw"), this, &APawn::AddControllerYawInput);

}

UPawnMovementComponent* ACollider::GetMovementComponent() const
{
	return MyMovementComponent;
}

UStaticMeshComponent* ACollider::GetMeshComponent()
{
	return MeshComponent;
}

void ACollider::SetMeshComponent(UStaticMeshComponent* Mesh)
{
	MeshComponent = Mesh;
}

void ACollider::MoveForward(float value)
{
	FVector forward = GetActorForwardVector();
	if (MyMovementComponent)
	{
		MyMovementComponent->AddInputVector(value*forward);
	}
}

void ACollider::MoveRight(float value)
{
	FVector right = GetActorRightVector();
	if (MyMovementComponent)
	{
		MyMovementComponent->AddInputVector(value*right,true);
	}
}

void ACollider::CameraPitch(float value)
{
	CameraInput.Y = value;
}

void ACollider::CameraYaw(float value)
{
	CameraInput.X = value;
}
