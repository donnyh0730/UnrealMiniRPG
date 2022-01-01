// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy_Boss.h"
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
#include "EnemyHealthBar.h"
#include "Engine/SkeletalMeshSocket.h"
AEnemy_Boss::AEnemy_Boss()
{
	ReactingSpeed = 2.0f;
	AttackSpeed = 1.5f;
	NormalAttackCount = 0;

	EnemyAttackStatus = EBossAttackStatus::EAS_Idle;

	LeftHandCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftHandCollision"));
	LeftHandCollision->SetupAttachment(GetMesh(), FName("LeftHandSocket"));

	RightHandCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("RightHandCollision"));
	RightHandCollision->SetupAttachment(GetMesh(), FName("RightHandSocket"));

	b_IsChampion = true;
}

void AEnemy_Boss::BeginPlay()
{
	Super::BeginPlay();

	RightHandCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightHandCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	RightHandCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	RightHandCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	RightHandCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemy_Boss::RightOnOverlapbegin);
	RightHandCollision->OnComponentEndOverlap.AddDynamic(this, &AEnemy_Boss::RightOnOverlapEnd);

	LeftHandCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftHandCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	LeftHandCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	LeftHandCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	LeftHandCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemy_Boss::LeftOnOverlapbegin);
	LeftHandCollision->OnComponentEndOverlap.AddDynamic(this, &AEnemy_Boss::LeftOnOverlapEnd);

}

void AEnemy_Boss::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemy_Boss::DamageReact_Anim(float In_Damage)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		if (CombatTarget)
		{
			if (CombatTarget->AttackStatus == EAttackStatus::EAS_Skill_1)
			{
				AnimInstance->Montage_Play(CombatMontage, ReactingSpeed);
				AnimInstance->Montage_JumpToSection(FName("Reaction2"), CombatMontage);
			}
		}
	}
}

void AEnemy_Boss::AttackMotion_Anim()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		if (NormalAttackCount < 3)
		{
			int32 Section = FMath::RandRange(0, 2);

			switch (Section)
			{
			case 0:
				AnimInstance->Montage_Play(CombatMontage, AttackSpeed);
				AnimInstance->Montage_JumpToSection(FName("Attack1"), CombatMontage);
				SetAttackStatus(EBossAttackStatus::EAS_Attack1);
				break;

			case 1:
				AnimInstance->Montage_Play(CombatMontage, AttackSpeed);
				AnimInstance->Montage_JumpToSection(FName("Attack2"), CombatMontage);
				SetAttackStatus(EBossAttackStatus::EAS_Attack2);
				break;

			case 2:
				AnimInstance->Montage_Play(CombatMontage, AttackSpeed);
				AnimInstance->Montage_JumpToSection(FName("Attack3"), CombatMontage);
				SetAttackStatus(EBossAttackStatus::EAS_Attack3);
				break;

			default:
				break;
			}
		}
		else
		{
			NormalAttackCount = 0;
			AnimInstance->Montage_Play(CombatMontage, AttackSpeed);
			AnimInstance->Montage_JumpToSection(FName("Skill"), CombatMontage);
		}
	}

}



void AEnemy_Boss::RightOnOverlapbegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	DamageProcess(OtherActor);
}

void AEnemy_Boss::RightOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	
}

void AEnemy_Boss::LeftOnOverlapbegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	DamageProcess(OtherActor);
}

void AEnemy_Boss::LeftOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void AEnemy_Boss::Attack_Boss()
{
	if (!Alive())
		return;

	if (AIController)
	{
		AIController->StopMovement();
		SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Attacking);
	}

	if(CombatTarget && bOverlappingCombatSphere)
		AttackMotion_Anim();

	if (!bAttacking)
	{
		bAttacking = true;
	}
	NormalAttackCount++;
}

void AEnemy_Boss::AttackEnd_Boss()
{
	bAttacking = false;
	SetAttackStatus(EBossAttackStatus::EAS_Idle);

	float AttackTime = FMath::FRandRange(AttackMintime, AttackMaxtime);
	GetWorldTimerManager().ClearTimer(AttackTimer);
	GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy_Boss::Attack_Boss, AttackTime);

	if (bOverlappingCombatSphere)
	{
		
	}
	else
	{
		if (CombatTarget)
		{
			MoveToTarget(CombatTarget);
		}
	}
	
}

void AEnemy_Boss::ActivateRightCollision()
{
	RightHandCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AEnemy_Boss::ActivateLeftCollision()
{
	LeftHandCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AEnemy_Boss::DeactivateCollision_Boss()
{
	RightHandCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftHandCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemy_Boss::BossReactionEnd()
{
	AttackEnd_Boss();
}

void AEnemy_Boss::SkillEnd()
{
	bAttacking = false;
	SetAttackStatus(EBossAttackStatus::EAS_Idle);
	if (CombatTarget)
	{
		float AttackTime = FMath::FRandRange(AttackMintime, AttackMaxtime);
		GetWorldTimerManager().ClearTimer(AttackTimer);
		GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy_Boss::Attack_Boss, AttackTime);
		MoveToTarget(CombatTarget);
	}
}

void AEnemy_Boss::DamageProcess(AActor* OtherActor)
{
	if (OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main)
		{
			if (Main->HitParticles)
			{
				FName SocketName;
				if(EnemyAttackStatus == EBossAttackStatus::EAS_Attack1)
					SocketName = "RightTipSocket";
				else if (EnemyAttackStatus == EBossAttackStatus::EAS_Attack2 || EnemyAttackStatus == EBossAttackStatus::EAS_Attack3)
				{
					SocketName = "LeftTipSocket";
				}
				
				const USkeletalMeshSocket* TipSocket = GetMesh()->GetSocketByName(SocketName);
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
			DeactivateCollision_Boss();
		}
	}
}

void AEnemy_Boss::Die()
{
	if (bisDying)
		return;

	bisDying = true;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->Montage_Play(CombatMontage, 1.0f);
		AnimInstance->Montage_JumpToSection(FName("Death"));
	}

	/*const FRotator Rotation = AIController->GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

	FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	Direction.Z = -0.5f;

	GetMesh()->SetCollisionResponseToAllChannels(ECR_Block);
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->AddImpulse(Direction*-1 * 10000);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);*/

	OnDeath_BP();

	SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Dead);
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AgroSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetWorldTimerManager().SetTimer(DeathTimer, this, &AEnemy_Boss::Disappear, DeathDelay);
}

void AEnemy_Boss::Disappear()
{
	OnDestroy_BP();
	Destroy();
}

