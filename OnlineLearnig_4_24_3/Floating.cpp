// Fill out your copyright notice in the Description page of Project Settings.


#include "Floating.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"

// Sets default values
AFloating::AFloating()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	
	bgotoStart = false;
	bgotoEnd = true;

	StartPoint = FVector(0.f);
	EndPoint = FVector(0.f);
	JourneyTime = 3.f;
}

// Called when the game starts or when spawned
void AFloating::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFloating::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFloating::MotoDestinationLocation(FVector Destination,float time)
{
	FVector Currentlocation = GetActorLocation();
	FVector Delta = Destination / (60.f * time);
	SetActorLocation(Currentlocation + Delta);
}


