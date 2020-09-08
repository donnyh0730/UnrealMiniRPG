// Fill out your copyright notice in the Description page of Project Settings.


#include "Main.h"
#include "MainPlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
#include <Components/InputComponent.h>
#include <GameFramework/Pawn.h>
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Engine/EngineTypes.h"
#include "Animation/AnimInstance.h"
#include "Weapon.h"
#include "Sound/SoundCue.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "TimerManager.h"
// Sets default values
AMain::AMain()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 600;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	RecognitionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("RecognitionSphere"));
	RecognitionSphere->SetupAttachment(GetRootComponent());


	BaseTurnRate = 65.f;
	BaseLookUpRate = 65.f;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);
	GetCharacterMovement()->JumpZVelocity = 550.f;
	GetCharacterMovement()->AirControl = 0.2f;


	MaxHealth = 100.f;
	Health = 65.f;
	MaxStamina = 350.f;
	Stamina = 120.f;
	Coins = 0;

	RunningSpeed = 650.f;
	SprintingSpeed = 950.f;

	bShiftKeyDown = false;
	bInterpToEnemy = false;
	InterpSpeed = 15.f;

	MovementStatus = EMovementStatus::EMS_Normal;
	StaminaStatus = EStaminaStatus::ESS_Normal;
	AttackStatus = EAttackStatus::EAS_Idle;
	WeaponStatus = EWeaponType::EAS_NoWeapon;

	StaminaDrainRate = 25.f;
	MinSprintStamina = 50.f;

	AttackSpeed = 2.5f;

	DeathDelay = 3.0f;

	bLMBDown = false;
	LMBDownCount = 0;
	bHasCombatTarget = false;

	bEquipedWeapon = false;
}

// Called when the game starts or when spawned
void AMain::BeginPlay()
{
	Super::BeginPlay();

	MainPlayerController = Cast<AMainPlayerController>(GetController());
	
	RecognitionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	RecognitionSphere->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	RecognitionSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	RecognitionSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	RecognitionSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Ignore);

	RecognitionSphere->OnComponentBeginOverlap.AddDynamic(this, &AMain::RecognitionSphereOnOverlapbegin);
	RecognitionSphere->OnComponentEndOverlap.AddDynamic(this, &AMain::RecognitionSphereOnOverlapEnd);

}

// Called every frame
void AMain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SwitchMovementStatus(StaminaStatus, DeltaTime);

	if (bInterpToEnemy)
	{
		FRotator LookAtYaw = GetControllerLookAtYawRotation();
		FRotator InterpRotation = FMath::RInterpTo(GetActorRotation(), LookAtYaw, DeltaTime, InterpSpeed);

		SetActorRotation(InterpRotation);
	}

	if (bStartCheckComboAttack)
	{
		if (LMBDownCount >= 2)
		{
			bComboAttack = true;
		}
	}
	DisplayAllEnemyWidgets();
}

// Called to bind functionality to input
void AMain::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMain::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AMain::ShiftKeyDown);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AMain::ShiftKeyUP);

	PlayerInputComponent->BindAction("LMB", IE_Pressed, this, &AMain::LMBDown);
	PlayerInputComponent->BindAction("LMB", IE_Released, this, &AMain::LMBUp);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMain::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMain::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAxis("TurnRate", this, &AMain::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMain::LookUpAtRate);


}

void AMain::SwitchMovementStatus(EStaminaStatus Status, float DeltaTime)
{
	if (MovementStatus == EMovementStatus::EMS_Dead)
		return;

	float DeltaStamina = StaminaDrainRate * DeltaTime;

	switch (Status)
	{
	case EStaminaStatus::ESS_Normal:
		if (bShiftKeyDown)
		{
			if (Stamina - DeltaStamina <= MinSprintStamina)
			{
				SetStaminaStatus(EStaminaStatus::ESS_BelowMinimum);
			}
			FVector Velocity = GetCharacterMovement()->Velocity;
			if(!GetCharacterMovement()->IsFalling() && Velocity.Size() != 0)
				Stamina -= DeltaStamina;
			SetMovementStatus(EMovementStatus::EMS_Sprinting);
		}
		else//shift키 업
		{
			if (Stamina + DeltaStamina >= MaxStamina)
			{
				Stamina = MaxStamina;
			}
			else
			{
				Stamina += DeltaStamina;
			}
			SetMovementStatus(EMovementStatus::EMS_Normal);
		}
		break;

	case EStaminaStatus::ESS_BelowMinimum:
		if (bShiftKeyDown)
		{
			if (Stamina - DeltaStamina <= 0.f)
			{
				Stamina = 0;
				SetStaminaStatus(EStaminaStatus::ESS_Exhausted);
				SetMovementStatus(EMovementStatus::EMS_Normal);
			}
			else
			{
				Stamina -= DeltaStamina;
				SetMovementStatus(EMovementStatus::EMS_Sprinting);
			}
		}
		else
		{
			if (Stamina + DeltaStamina >= MinSprintStamina)
			{
				SetStaminaStatus(EStaminaStatus::ESS_Normal);
				Stamina += DeltaStamina;
			}
			else
			{
				Stamina += DeltaStamina;
			}
			SetMovementStatus(EMovementStatus::EMS_Normal);
		}
		break;

	case EStaminaStatus::ESS_Exhausted:
		if (bShiftKeyDown)
		{
			Stamina = 0;
		}
		else//shift키 업
		{
			SetStaminaStatus(EStaminaStatus::ESS_ExhaustedRecovering);
			Stamina += DeltaStamina;
		}
		SetMovementStatus(EMovementStatus::EMS_Normal);
		break;

	case EStaminaStatus::ESS_ExhaustedRecovering:
		if (Stamina + DeltaStamina >= MinSprintStamina)
		{
			SetStaminaStatus(EStaminaStatus::ESS_Normal);
			Stamina += DeltaStamina;
		}
		else
		{
			Stamina += DeltaStamina;
		}
		SetMovementStatus(EMovementStatus::EMS_Normal);
		break;

	case EStaminaStatus::ESS_MAX:
		break;
	}
}

void AMain::MoveForward(float value)
{
	if (MovementStatus == EMovementStatus::EMS_Dead)
		return;
	if (Controller != nullptr && value != 0.f && !bAttacking)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, value);
	}
}

void AMain::MoveRight(float value)
{
	if (MovementStatus == EMovementStatus::EMS_Dead)
		return;
	if (Controller != nullptr && value != 0.f && !bAttacking)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, value);
	}
}

void AMain::TurnAtRate(float rate)
{
	AddControllerYawInput(rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMain::LookUpAtRate(float rate)
{
	AddControllerPitchInput(rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AMain::Jump()
{
	if (MovementStatus != EMovementStatus::EMS_Dead)
	{
		Super::Jump();
	}
}

void AMain::LMBDown()
{
	bLMBDown = true;
	if (MovementStatus == EMovementStatus::EMS_Dead) 
		return;

	if (ActiveOverlappingItem)
	{
		AWeapon* Weapon = Cast<AWeapon>(ActiveOverlappingItem);
		if (Weapon)
		{
			Weapon->Equip(this);
			ActiveOverlappingItem = nullptr;
		}
	}
	else if (WeaponStatus==EWeaponType::EAS_OneHand || WeaponStatus==EWeaponType::EAS_DualBlade)
	{
		//Attack();
		ComboAttack();
		LMBDownCount++;
	}
}

void AMain::LMBUp()
{
	bLMBDown = false;
}

void AMain::DetachWeapon()
{
	if (RightEquippedWeapon && LeftEquippedWeapon)
	{
		const FVector location = GetActorLocation();
		const FRotator Rotation = FRotator(0.f);
		
		if (RightEquippedWeapon)
		{
			GetWorld()->SpawnActor(RightEquippedWeapon->GetClass(), &location, &Rotation);
			RightEquippedWeapon->Destroy();

			return;
		}
		if (LeftEquippedWeapon)
		{
			GetWorld()->SpawnActor(LeftEquippedWeapon->GetClass(), &location, &Rotation);
			LeftEquippedWeapon->Destroy();
			return;
		}
	}
	if (!RightEquippedWeapon && !LeftEquippedWeapon)
	{
		bEquipedWeapon = false;
	}
}

void AMain::SetEquippedWeapon(AWeapon* Weapon)
{
	DetachWeapon();
	if (!RightEquippedWeapon)
	{
		RightEquippedWeapon = Weapon;
		DeterminWeaponStatus();
		return;
	}
	else if(!LeftEquippedWeapon)
	{
		LeftEquippedWeapon = Weapon; 
		DeterminWeaponStatus();
		return;
	}
	
}

void AMain::DeterminWeaponStatus()
{
	if (!RightEquippedWeapon && !LeftEquippedWeapon)
	{
		WeaponStatus = EWeaponType::EAS_NoWeapon;
		bEquipedWeapon = false;
	}
	else if (RightEquippedWeapon && !LeftEquippedWeapon)
	{
		WeaponStatus = EWeaponType::EAS_OneHand;
		bEquipedWeapon = true;
	}
	else if (RightEquippedWeapon && LeftEquippedWeapon)
	{
		WeaponStatus = EWeaponType::EAS_DualBlade;
		bEquipedWeapon = true;
	}
}

void AMain::PlayAttackAnimation(EWeaponType WeaponType)
{
	switch (WeaponType)
	{
	case EWeaponType::EAS_NoWeapon:
		break;
	case EWeaponType::EAS_OneHand:
		SwitchAttackSequence(AttackStatus, OneHandMontages);
		break;
	case EWeaponType::EAS_DualBlade:
		SwitchAttackSequence(AttackStatus, DualBladeMontages);
		break;
	}

}

void AMain::ComboAttackStart()
{
	UE_LOG(LogTemp, Warning, TEXT("ComboAttackStart()"));
	bStartCheckComboAttack = true;
	bComboAttack = false;

	bAttacking = true;
}

void AMain::ComboAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("ComboAttack()"));
	//if (bStartCheckComboAttack)
	//{
	//	//bComboAttack = true;
	//}

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (!IsValid(AnimInstance))
	{
		return;
	}

	SetInterpToEnemy(true);

	if (AttackStatus == EAttackStatus::EAS_Idle && MovementStatus != EMovementStatus::EMS_Dead)
	{
		/*SetAttackStatus(EAttackStatus::EAS_Attack_1);
		AnimInstance->Montage_Play(OneHandMontages[0], AttackSpeed);
		AnimInstance->Montage_JumpToSection(FName("Default"), OneHandMontages[0]);*/
		PlayAttackAnimation(WeaponStatus);
	}
}

void AMain::ComboAttackEnd()
{
	UE_LOG(LogTemp, Warning, TEXT("ComboAttackEnd()"));
	if (bComboAttack)
	{
		PlayAttackAnimation(WeaponStatus);
	}
	if (!bComboAttack)
	{
		SetAttackStatus(EAttackStatus::EAS_Idle);
		//bAttacking = false;
	}
	bStartCheckComboAttack = false;
	bComboAttack = false;
	LMBDownCount = 0;
}

void AMain::PlaySwingSound()
{
	if (RightEquippedWeapon->SwingSound)
	{
		UGameplayStatics::PlaySound2D(this, RightEquippedWeapon->SwingSound);
	}
}

void AMain::SwitchAttackSequence(EAttackStatus Status, UAnimMontage* MontageSet)
{
	if (MovementStatus == EMovementStatus::EMS_Dead)
		return;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	
	if (!IsValid(AnimInstance))
	{
		return;
	}
	
	switch (Status)
	{
	case EAttackStatus::EAS_Idle:
		SetAttackStatus(EAttackStatus::EAS_Attack_1);
		AnimInstance->Montage_Play(MontageSet, AttackSpeed);
		AnimInstance->Montage_JumpToSection(FName("Attack_1"), MontageSet);
		break;
	case EAttackStatus::EAS_Attack_1:
		SetAttackStatus(EAttackStatus::EAS_Attack_2);
		AnimInstance->Montage_Play(MontageSet, AttackSpeed);
		AnimInstance->Montage_JumpToSection(FName("Attack_2"), MontageSet);
		break;
	case EAttackStatus::EAS_Attack_2:
		SetAttackStatus(EAttackStatus::EAS_Attack_3);
		AnimInstance->Montage_Play(MontageSet, AttackSpeed);
		AnimInstance->Montage_JumpToSection(FName("Attack_3"), MontageSet);
		break;
	case EAttackStatus::EAS_Attack_3:
		SetAttackStatus(EAttackStatus::EAS_Idle);
		
		bAttacking = false;
		break;

	default:
		break;
	}
}

void AMain::DisplayAllEnemyWidgets()
{
	for (AEnemy* item : RecognizedEnemies)
	{
		item->HealthBarProjectToViewport();
		item->DisplayEnemyHealthBar();
	}

}

void AMain::RecognitionSphereOnOverlapbegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("RecognitionSphereOnOverlapbegin"));
	if (OtherActor)
	{
		AEnemy* Enemy = Cast<AEnemy>(OtherActor);
		if (Enemy)
		{
			Enemy->CreateWidget();
			RecognizedEnemies.Add(Enemy);
		}
	}
}

void AMain::RecognitionSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("RecognitionSphereOnOverlapEnd"));
	if (OtherActor)
	{
		AEnemy* Enemy = Cast<AEnemy>(OtherActor);
		if (Enemy)
		{
			Enemy->RemoveWidget();
			RecognizedEnemies.Remove(Enemy);
		}
	}
}

void AMain::Attack()
{
	if (!bAttacking)
	{
		bAttacking = true;

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		/*const FRotator Rotation = Controller->GetControlRotation();
		FRotator newrotation(0.f, Rotation.Yaw, 0.f);
		SetActorRotation(newrotation);*/
		if (AnimInstance && BaseMotionMontage)
		{
			int32 Section = FMath::RandRange(0, 1);

			switch (Section)
			{
			case 0:
				AnimInstance->Montage_Play(BaseMotionMontage, 2.2f);
				AnimInstance->Montage_JumpToSection(FName("Attack_1"), BaseMotionMontage);
				break;
			case 1:
				AnimInstance->Montage_Play(BaseMotionMontage, 1.8f);
				AnimInstance->Montage_JumpToSection(FName("Attack_2"), BaseMotionMontage);
				break;
			default:
				break;
			}

		}
		if (RightEquippedWeapon->SwingSound)
		{
			UGameplayStatics::PlaySound2D(this, RightEquippedWeapon->SwingSound);
		}
	}
}

void AMain::AttackEnd()
{
	UE_LOG(LogTemp, Warning, TEXT("AttackEnd()"));
	bAttacking = false;
	SetInterpToEnemy(false);
	/*if (bLMBDown)
	{
		Attack();
	}*/
}

void AMain::SetMovementStatus(EMovementStatus Status)
{
	MovementStatus = Status;
	switch (Status)
	{
	case EMovementStatus::EMS_MAX:
		break;

	case EMovementStatus::EMS_Normal:
		GetCharacterMovement()->MaxWalkSpeed = RunningSpeed;
		break;
	case EMovementStatus::EMS_Sprinting:
		GetCharacterMovement()->MaxWalkSpeed = SprintingSpeed;
		break;
	}
}

void AMain::SetAttackStatus(EAttackStatus Status)
{
	AttackStatus = Status;
}

void AMain::ShiftKeyDown()
{
	bShiftKeyDown = true;
}

void AMain::ShiftKeyUP()
{
	bShiftKeyDown = false;
}

void AMain::SetInterpToEnemy(bool bInterp)
{
	bInterpToEnemy = bInterp;
}

FRotator AMain::GetControllerLookAtYawRotation()
{
	FRotator Rotation = Controller->GetControlRotation();
	FRotator LookAtRotationYaw(0.f, Rotation.Yaw, 0.f);
	return LookAtRotationYaw;
}

FRotator AMain::GetLookTargetYawRotaion(FVector TargetLocation)
{
	FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), TargetLocation);
	FRotator LookAtRotationYaw(0.f, Rotation.Yaw, 0.f);
	return LookAtRotationYaw;
}

void AMain::IncrementCoins(int32 Amount)
{
	Coins += Amount;
}

void AMain::DecrementHealth(float Amount)
{
	Health -= Amount;
	if (Health - Amount <= 0.f)
	{
		Die();
	}
}

float AMain::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	DecrementHealth(DamageAmount);

	return DamageAmount;
}

void AMain::Die()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && BaseMotionMontage)
	{
		AnimInstance->Montage_Play(BaseMotionMontage, 1.0f);
		AnimInstance->Montage_JumpToSection(FName("Death"), BaseMotionMontage);
	}
	SetMovementStatus(EMovementStatus::EMS_Dead);

	RecognitionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AMain::DeathEnd()
{
	GetMesh()->bPauseAnims = true;
	GetMesh()->bNoSkeletonUpdate = true;

	GetWorldTimerManager().SetTimer(DeathTimer, this, &AMain::Disappear, DeathDelay);
}

void AMain::Disappear()
{

}
