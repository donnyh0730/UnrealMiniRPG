// Fill out your copyright notice in the Description page of Project Settings.


#include "FloorSwitch.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Main.h"
#include "TimerManager.h"
// Sets default values
AFloorSwitch::AFloorSwitch()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	RootComponent = TriggerBox;

	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	TriggerBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	TriggerBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn,ECollisionResponse::ECR_Overlap);

	TriggerBox->SetBoxExtent(FVector(62.f, 62.f, 32.f));

	FloorSwitch = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FloorSwitch"));
	FloorSwitch->SetupAttachment(GetRootComponent());

	Door = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door"));
	Door->SetupAttachment(GetRootComponent());

	MovingDistance = 450.f;
	CurrentMoveDistance = 0.f;

	bRaising = false;
	bLowering = false;
}

// Called when the game starts or when spawned
void AFloorSwitch::BeginPlay()
{
	Super::BeginPlay();
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AFloorSwitch::OnOverlapBegin);
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &AFloorSwitch::OnOverlapEnd);

	DoorInitiallocation = Door->GetComponentLocation();
	SwitchInitiallocation = FloorSwitch->GetComponentLocation();

	
}

// Called every frame
void AFloorSwitch::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	//AnimatingStuff();
}

void AFloorSwitch::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Overlap Begin"));
	bIsOnPawn = true;
	
	if (OtherActor)
	{
		AMain* main = Cast<AMain>(OtherActor);
		if (main)
		{
			Main = main; 
			RaiseDoor();
			LowerFloorSwitch();
		}
	}
	
}

void AFloorSwitch::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("Overlap End %f seconds") , 2.0f);
	
	bIsOnPawn = false;
	if (OtherActor)
	{
		AMain* main = Cast<AMain>(OtherActor);
		if (main)
		{
			Main = main;
		}
	}
	LowerDoorInSeconds(2.0f);
	RaiseFloorSwitch();
	
}

void AFloorSwitch::UpdateDoorLocation(float Z)
{
	FVector newlocation = DoorInitiallocation;
	newlocation.Z += Z;
	Door->SetWorldLocation(newlocation);
}

void AFloorSwitch::UpdateFloorSwitchLocation(float Z)
{
	FVector newlocation = SwitchInitiallocation;
	newlocation.Z += Z;
	FloorSwitch->SetWorldLocation(newlocation);
}


void AFloorSwitch::LowerDoorInSeconds(float sec)
{
	GetWorldTimerManager().ClearTimer(LowerDoorTimerHandler);
	GetWorldTimerManager().SetTimer(LowerDoorTimerHandler, this, &AFloorSwitch::CloseDoor, sec, false);

}

void AFloorSwitch::CloseDoor()
{
	if (!bIsOnPawn)
	{

		LowerDoor();
	}
}

//void AFloorSwitch::RaiseDoor()
//{
//	float movetick = MovingDistance / 60.f;
//	FVector DoorLocation = Door->GetComponentLocation();
//	FVector newlocation(DoorLocation.X, DoorLocation.Y, DoorLocation.Z + movetick);
//	CurrentMoveDistance += movetick;
//	if (CurrentMoveDistance > MovingDistance)
//	{
//		CurrentMoveDistance = 0.f;
//		bRaising = false;
//		newlocation.Z = DoorInitiallocation.Z + MovingDistance;
//	}
//	Door->SetWorldLocation(newlocation);
//	
//	
//}
//
//void AFloorSwitch::LowerDoor()
//{
//	float movetick = MovingDistance / 60.f;
//	FVector DoorLocation = Door->GetComponentLocation();
//	FVector newlocation(DoorLocation.X, DoorLocation.Y, DoorLocation.Z - movetick);
//	CurrentMoveDistance += movetick;
//	if (CurrentMoveDistance > MovingDistance)
//	{
//		CurrentMoveDistance = 0.f;
//		bLowering = false;
//		newlocation.Z = DoorInitiallocation.Z;
//	}
//	Door->SetWorldLocation(newlocation);
//}
//
//void AFloorSwitch::RaiseFloorSwitch()
//{
//
//}
//
//void AFloorSwitch::LowerFloorSwitch()
//{
//
//}
//
//void AFloorSwitch::AnimatingStuff()
//{
//	if (bRaising)
//	{
//		RaiseDoor();
//	}
//	if (bLowering)
//	{
//		LowerDoor();
//	}
//}

