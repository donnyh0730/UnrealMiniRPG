// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Main.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Enemy.h"
#include "Engine/SkeletalMeshSocket.h"
AWeapon::AWeapon()
{
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMesh->SetupAttachment(GetRootComponent());

	CombatCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("CombatCollision"));
	CombatCollision->SetupAttachment(GetRootComponent());

	SkillCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SkillCollision"));
	SkillCollision->SetupAttachment(GetRootComponent());

	bWeaponParticle = false;

	WeaponState = EWeaponState::EWS_Pickup;

	WeaponAttackSpeed = 2.0f;
	Damage = 25.f;
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CombatCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	SkillCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	CombatCollision->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::CombatOnOverlapbegin);
	CombatCollision->OnComponentEndOverlap.AddDynamic(this, &AWeapon::CombatOnOverlapEnd);

	SkillCollision->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::SkillOnOverlapbegin);
	SkillCollision->OnComponentEndOverlap.AddDynamic(this, &AWeapon::SkillOnOverlapEnd);
}

float AWeapon::CalcDemageConstvalue()
{
	float val = 1;

	if (OwnerCharacter->AttackStatus == EAttackStatus::EAS_Skill_1)
	{
		val = 1.0f;
		return val;
	}
	else if (OwnerCharacter->AttackStatus == EAttackStatus::EAS_Attack_1)
	{
		val = 1.0f;
		return val;
	}
	else if (OwnerCharacter->AttackStatus == EAttackStatus::EAS_Attack_2)
	{
		val = 1.2f;
		return val;
	}
	else if (OwnerCharacter->AttackStatus == EAttackStatus::EAS_Attack_3)
	{
		val = 1.4f;
		return val;
	}
	return val;
}

void AWeapon::OnOverlapbegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	Super::OnOverlapbegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if (WeaponState == EWeaponState::EWS_Pickup && OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main)
		{
			Main->SetActiveOverlappingItem(this);
			Main->VisibleInfoWidget(ESlateVisibility::Visible);
			Main->SetInfoText(6);
		}
	}
}

void AWeapon::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
	if (OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main)
		{
			Main->SetActiveOverlappingItem(nullptr);
			Main->VisibleInfoWidget(ESlateVisibility::Hidden);
		}
	}
}

void AWeapon::CombatOnOverlapbegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	DamageProcess(OtherActor);
}

void AWeapon::CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	
}

void AWeapon::SkillOnOverlapbegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	DamageProcess(OtherActor);
}

void AWeapon::SkillOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void AWeapon::ActivateCollision()
{
	if(!OwnerCharacter)
		return;
	if (OwnerCharacter->AttackStatus == EAttackStatus::EAS_Skill_1)
	{
		SkillCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
	else
	{
		CombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
}

void AWeapon::DeactivateCollision()
{
	if (!OwnerCharacter)
		return;
	
	SkillCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
}

void AWeapon::DamageProcess(AActor* OtherActor)
{
	if (OtherActor)
	{
		AEnemy* Enemy = Cast<AEnemy>(OtherActor);
		if (Enemy)
		{
			if (Enemy->HitParticles)
			{
				const USkeletalMeshSocket* PariticleSocket = SkeletalMesh->GetSocketByName("HitParticleSocket");
				if (PariticleSocket)
				{
					FVector SocketLocation = PariticleSocket->GetSocketLocation(SkeletalMesh);
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Enemy->HitParticles, SocketLocation, FRotator(0.f), false);
				}

			}
			if (Enemy->HitSound)
			{
				UGameplayStatics::PlaySound2D(this, Enemy->HitSound);
			}
			if (DamageTypeClass)
			{
				if (Enemy->Health - Damage <= 0)
				{
					Enemy->GetMesh()->SetCollisionResponseToAllChannels(ECR_Block);
					Enemy->GetMesh()->SetSimulatePhysics(true);
					Enemy->GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
				}
				float constval = CalcDemageConstvalue();

				UGameplayStatics::ApplyDamage(Enemy, Damage * constval, WeaponInstigator, this, DamageTypeClass);
			}

		}
	}
}

void AWeapon::SetMainCharacterOwner(AMain* _OwnerCharacter)
{
	OwnerCharacter = _OwnerCharacter;
}

void AWeapon::Equip(AMain* Character)
{
	if (Character)
	{
		CollisionVolume->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		SetInstigator(Character->GetController());

		SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
		SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

		SkeletalMesh->SetSimulatePhysics(false);

		const USkeletalMeshSocket* RighetHandSocket = Character->GetMesh()->GetSocketByName("RightHandSocket");
		RighetHandSocket->AttachActor(this, Character->GetMesh());
		/*if (!Character->RightEquippedWeapon)
		{

		}
		else if (!Character->LeftEquippedWeapon)
		{
			const USkeletalMeshSocket* LeftHandSocket = Character->GetMesh()->GetSocketByName("LeftHandSocket");
			LeftHandSocket->AttachActor(this, Character->GetMesh());
		}*/
		bRotate = false;
		
		Character->SetEquippedWeapon(this);
		Character->SetActiveOverlappingItem(nullptr);
		if (OnEquipSound)
		{
			UGameplayStatics::PlaySound2D(this, OnEquipSound);
		}
		if (!bWeaponParticle)
		{
			//IdleParticlesComponent->Deactivate();
		}
	}
}
