// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "AIController.h"
#include "Main.h"
#include "MainPlayerController.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Weapon.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Sound/SoundCue.h"
#include "Animation/AnimInstance.h"
#include "TimerManager.h"
#include "Components/CapsuleComponent.h"
#include "Blueprint/UserWidget.h"
#include "EnemyHealthBar.h"
// Sets default values
AEnemy::AEnemy()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AgroSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AgroSphere"));
	AgroSphere->SetupAttachment(GetRootComponent());

	AgroSphere->InitSphereRadius(600.f);

	CombatSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CombatSphere"));
	CombatSphere->SetupAttachment(GetRootComponent());
	CombatSphere->InitSphereRadius(75.f);

	CombatCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("CombatCollision"));
	CombatCollision->SetupAttachment(GetMesh(), FName("EnemySocket"));
	//CombatCollision->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("EnemySocket"));

	Health = 100.f;
	MaxHealth = 100.f;
	Damage = 10.f;
	AttackSpeed = 1.35f;
	ReactingSpeed = 1.0f;

	AttackMintime = 0.5f;
	AttackMaxtime = 3.5f;

	bHasTarget = false;
	EnemyMovementStatus = EEnemyMovementStatus::EMS_Idle;

	DeathDelay = 3.0f;
	bisDying = false;
	b_IsChampion = false;
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	AIController = Cast<AAIController>(GetController());

	AgroSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::AgroSphereOnOverlapbegin);
	AgroSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::AgroSphereOnOverlapEnd);

	CombatSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::CombatSphereOnOverlapbegin);
	CombatSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::CombatSphereOnOverlapEnd);
	bOverlappingCombatSphere = false;

	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CombatCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	CombatCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::CombatOnOverlapbegin);
	CombatCollision->OnComponentEndOverlap.AddDynamic(this, &AEnemy::CombatOnOverlapEnd);

	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
}

void AEnemy::DamageReact_Anim(float In_Damage)
{

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		if (b_IsChampion)
		{
			if (CombatTarget)
			{
				if (CombatTarget->AttackStatus == EAttackStatus::EAS_Skill_1)
				{
					AnimInstance->Montage_Play(CombatMontage, ReactingSpeed * 0.7f);
					AnimInstance->Montage_JumpToSection(FName("AttackReactions"), CombatMontage);
				}
				else
				{

				}
			}
		}
		else
		{
			AnimInstance->Montage_Play(CombatMontage, ReactingSpeed);
			AnimInstance->Montage_JumpToSection(FName("AttackReaction"), CombatMontage);
		}
	}
}

void AEnemy::AttackMotion_Anim()
{

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance)
	{
		AnimInstance->Montage_Play(CombatMontage, AttackSpeed);
		AnimInstance->Montage_JumpToSection(FName("Attack"), CombatMontage);

	}
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (EnemyMovementStatus == EEnemyMovementStatus::EMS_Dead)
	{
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	if (EnemyMovementStatus == EEnemyMovementStatus::EMS_Idle)
	{
		if (bOverlappingCombatSphere || CombatTarget)
		{
			MoveToTarget(CombatTarget);
		}
	}
}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::AgroSphereOnOverlapbegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor && Alive())
	{
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main)
		{
			CombatTarget = Main;
			bHasTarget = true;
			MoveToTarget(Main);
		}
	}
}

void AEnemy::AgroSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main)
		{
			CombatTarget = nullptr;
			bHasTarget = false;
			
			if (AIController)
			{
				AIController->StopMovement();
			}
		}

	}

}

void AEnemy::CombatSphereOnOverlapbegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor && Alive())
	{
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main)
		{
			CombatTarget = Main;
			
			bOverlappingCombatSphere = true;
			Attack();
		}
		/*AWeapon* Weapon = Cast<AWeapon>(OtherActor);
		if (Weapon)
		{
			AttackingWeapon = Weapon;
		}*/
	}
}

void AEnemy::CombatSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main)
		{
			bOverlappingCombatSphere = false;
			if (EnemyMovementStatus != EEnemyMovementStatus::EMS_Attacking)
				//애님 노티파이에 의해서 movetotarget이 호출되면 현재상태는 Attacking이 아닌 MoveToTarget이기때문에 이if문이 true가 되면서 다시 타겟에게 움직이게 된다.
			{
				MoveToTarget(Main);
			}
			GetWorldTimerManager().ClearTimer(AttackTimer);
		}
	}
}

void AEnemy::CombatOnOverlapbegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	DamageProcess(OtherActor);
}

void AEnemy::CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void AEnemy::SwingStart()
{
	if (SwingSound)
	{
		UGameplayStatics::PlaySound2D(this, SwingSound);
	}
}

void AEnemy::ActivateCollision()
{
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AEnemy::DeactivateCollision()
{
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemy::MoveToTarget(AMain* Target)
{
	SetEnemyMovementStatus(EEnemyMovementStatus::EMS_MoveToTarget);

	if (AIController)
	{
		FAIMoveRequest MoveRequest;

		MoveRequest.SetGoalActor(Target);
		MoveRequest.SetAcceptanceRadius(5.0f);

		FNavPathSharedPtr NavPath;
		AIController->MoveTo(MoveRequest, &NavPath);

	}
}


void AEnemy::DamageProcess(AActor* OtherActor)
{
	if (OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main)
		{
			if(Main->AttackStatus == EAttackStatus::EAS_Skill_1)
			{
				CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				return;
			}
			
			if (Main->HitParticles)
			{
				const USkeletalMeshSocket* TipSocket = GetMesh()->GetSocketByName("TipSocket");
				if (TipSocket)
				{
					FVector SocketLocation = TipSocket->GetSocketLocation(GetMesh());
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Main->HitParticles, SocketLocation, FRotator(0.f), false);
				}

			}
			if (Main->HitSound)
			{
				UGameplayStatics::PlaySound2D(this, Main->HitSound);
			}
			if (DamageTypeClass)
			{
				UGameplayStatics::ApplyDamage(Main, Damage, AIController, this, DamageTypeClass);
				//ApplyDamage가 호출되면 대상 액터의 TakeDamage가 호출된다.
			}
			CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
}

void AEnemy::Attack()
{
	if (!Alive())
		return;

	if (AIController)
	{
		AIController->StopMovement();
		SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Attacking);
	}
	AttackMotion_Anim();
	if (!bAttacking)
	{
		bAttacking = true;
	}

}

void AEnemy::AttackEnd()
{
	bAttacking = false;
	SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Idle);
	if (bOverlappingCombatSphere)
	{
		float AttackTime = FMath::FRandRange(AttackMintime, AttackMaxtime);
		GetWorldTimerManager().ClearTimer(AttackTimer);
		GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, AttackTime);
		//Attack();
	}
	else
	{
		if (CombatTarget)
		{
			MoveToTarget(CombatTarget);
		}
	}

}


float AEnemy::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	Health -= DamageAmount;
	if (Health <= 0.f)
	{
		Die();
	}
	if (Health > 0.f)
	{
		SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Falling);
		DamageReact_Anim(DamageAmount);
	}

	return DamageAmount;
}

void AEnemy::CalcDamage(float Amount)
{

}

void AEnemy::Die()
{
	if (bisDying)
		return;

	bisDying = true;
	UE_LOG(LogTemp, Warning, TEXT("Die()"));
	/*UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->Montage_Play(CombatMontage, 1.0f);
		AnimInstance->Montage_JumpToSection(FName("Death"));
	}*/

	/*const FRotator Rotation = AIController->GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

	FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	Direction.Z = -0.5f;

	GetMesh()->SetCollisionResponseToAllChannels(ECR_Block);
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->AddImpulse(Direction*-1 * 10000);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);*/

	OnDeath_BP();

	if (FinalHitSound)
	{
		UGameplayStatics::PlaySound2D(this, FinalHitSound);
	}

	SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Dead);
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AgroSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GetWorldTimerManager().SetTimer(DeathTimer, this, &AEnemy::Disappear, DeathDelay);
}

void AEnemy::DeathEnd()
{
	//GetMesh()->bPauseAnims = true;
	//GetMesh()->bNoSkeletonUpdate = true;

	GetWorldTimerManager().SetTimer(DeathTimer, this, &AEnemy::Disappear, DeathDelay);
}

bool AEnemy::Alive()
{
	return GetEnemyMovementStatus() != EEnemyMovementStatus::EMS_Dead;
}

void AEnemy::Disappear()
{
	OnDestroy_BP();
	Destroy();
}

void AEnemy::AttackReactionEnd()
{
	AttackEnd();
}

void AEnemy::CreateWidget()
{
	AMainPlayerController* MainPlayerController = Cast<AMainPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	if (MainPlayerController)
	{
		MainPlayerController->AttachWidgettoNewEnemy(this);
	}
}

void AEnemy::HealthBarProjectToViewport()
{
	if (EnemyHealthBar)
	{
		FVector2D PositionInViewport;

		AMainPlayerController* MainPlayerController = Cast<AMainPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
		if (MainPlayerController)
			MainPlayerController->ProjectWorldLocationToScreen(GetActorLocation(), PositionInViewport);

		FVector2D SizeInViewPort(300.f, 25.f);

		EnemyHealthBar->SetHealth(Health);
		EnemyHealthBar->SetMaxHealth(MaxHealth);

		EnemyHealthBar->SetPositionInViewport(PositionInViewport);
		EnemyHealthBar->SetDesiredSizeInViewport(SizeInViewPort);
	}
}

void AEnemy::DisplayEnemyHealthBar()
{
	if (EnemyHealthBar)
	{
		EnemyHealthBar->SetVisibility(ESlateVisibility::Visible);
	}
}

void AEnemy::RemoveWidget()
{
	AMainPlayerController* MainPlayerController = Cast<AMainPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	if (MainPlayerController)
	{
		MainPlayerController->DeleteWidgetFromEnemy(this);
	}
}
