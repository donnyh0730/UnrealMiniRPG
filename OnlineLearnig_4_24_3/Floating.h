// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Floating.generated.h"


UCLASS()
class ONLINELEARNIG_4_24_3_API AFloating : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFloating();

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Mesh")
	UStaticMeshComponent* Mesh;

	float Period;
	UPROPERTY(EditAnywhere)
	FVector StartPoint;
	UPROPERTY(EditAnywhere, meta = (MakeEditWidget = "true"))
	FVector EndPoint;

	FVector DeltaPosition;

	float JourneyTime;

	bool bgotoStart;
	bool bgotoEnd;

	FTimerHandle TimergotoStart;

	FTimerHandle TimergotoEnd;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void MotoDestinationLocation(FVector Destination, float time);

};
