// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC_Character.h"
#include "AIController.h"
#include "Main.h"
#include "MainPlayerController.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Sound/SoundCue.h"
#include "TimerManager.h"
#include "Components/CapsuleComponent.h"
#include "Blueprint/UserWidget.h"
#include "EnemyHealthBar.h"
// Sets default values
ANPC_Character::ANPC_Character()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RecognitionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("RecognitionSphere"));
	RecognitionSphere->SetupAttachment(GetRootComponent());
	RecognitionSphere->InitSphereRadius(400.f);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	Camera->SetupAttachment(GetRootComponent());

	MeassageCount = 0;
	MaxMessageCount = 8;
	b_NearCharacter = false;
}

// Called when the game starts or when spawned
void ANPC_Character::BeginPlay()
{
	Super::BeginPlay();
	
	RecognitionSphere->OnComponentBeginOverlap.AddDynamic(this, &ANPC_Character::RecognitionSphereOnOverlapbegin);
	RecognitionSphere->OnComponentEndOverlap.AddDynamic(this, &ANPC_Character::RecognitionSphereOnOverlapEnd);

}

// Called every frame
void ANPC_Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ANPC_Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ANPC_Character::RecognitionSphereOnOverlapbegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	//Press 'E' 출력 UI On 아마도 위젯 visible 하고 string table기반 다이얼로그 진행
	if (OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main)
		{
			Main->PrepareNearNPC(this,true,ESlateVisibility::Visible);
			b_NearCharacter = true;
			MainCharacter = Main;
		}
	}
}
	

void ANPC_Character::RecognitionSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//Press 'E' 출력 UI Off
	if (OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main)
		{
			Main->PrepareNearNPC(this, false, ESlateVisibility::Hidden);
			b_NearCharacter = false;
			MainCharacter = nullptr;
		}
	}

}

void ANPC_Character::OnInteractInBase()
{
	if (MeassageCount >= MaxMessageCount)
	{
		MeassageCount = 0;
		if(MainCharacter && MainCharacter->MainPlayerController)
		{
			AMainPlayerController* controller = MainCharacter->MainPlayerController;

			controller->SetVisibleDialogueWidget(ESlateVisibility::Hidden);
			controller->SetVisibleInfoWidget(ESlateVisibility::Visible);
		}
	}
	else
	{
		MeassageCount++;
		OnInteract();
	}
	
}

