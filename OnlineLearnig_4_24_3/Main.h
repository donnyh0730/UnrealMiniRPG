// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy.h"
#include "Blueprint/UserWidget.h"
#include "Main.generated.h"

class USpringArmComponent;
class UCameraComponent;
class AWeapon;
class AItem;
class UAnimMontage;
class UParticleSystemComponent;
class USoundCue;
class AEnemy;
class AMainPlayerController;
class USphereComponent;
class ANPC_Character;

UENUM(BlueprintType)
enum class EMovementStatus : uint8
{
	EMS_Normal UMETA(DisplayName = "Noraml"),
	EMS_Sprinting UMETA(DisplayName = "Sprinting"),
	EMS_Rolling UMETA(DisplayName = "Rolling"),
	EMS_Dead UMETA(DisplayName = "Dead"),

	EMS_MAX UMETA(DisplayName ="DefaultMax"),
};

UENUM(BlueprintType)
enum class EStaminaStatus : uint8
{
	ESS_Normal UMETA(DisplayName = "Normal"),
	ESS_BelowMinimum UMETA(DisplayName = "BelowMinimum"),
	ESS_Exhausted UMETA(DisplayName = "Exhausted"),
	ESS_ExhaustedRecovering UMETA(DisplayName = "ExhaustedRecovering"),
	ESS_MAX UMETA(DisplayName = "DefaultMax"),

};

UENUM(BlueprintType)
enum class EAttackStatus : uint8
{
	EAS_Idle UMETA(DisplayName ="Idle"),
	EAS_Attack_1 UMETA(DisplayName = "Attack_1"),
	EAS_Attack_2 UMETA(DisplayName = "Attack_2"),
	EAS_Attack_3 UMETA(DisplayName = "Attack_3"),
	EAS_Skill_1  UMETA(DisplayName = "Skill_1"),
	EAS_MAX UMETA(DisplayName = "DefaultMax"),
};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EAS_NoWeapon UMETA(DisplayName = "NoWeapon"),
	EAS_OneHand UMETA(DisplayName = "OneHand"),
	EAS_DualBlade UMETA(DisplayName = "DualBlade"),
	
	EAS_MAX UMETA(DisplayName = "DefaultMax"),
};

UCLASS()
class ONLINELEARNIG_4_24_3_API AMain : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMain();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Controller")
	AMainPlayerController* MainPlayerController;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite , Category = "Enums")
	EMovementStatus MovementStatus;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
	EStaminaStatus StaminaStatus;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
	EAttackStatus AttackStatus;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
	EWeaponType WeaponStatus;

	FORCEINLINE void SetStaminaStatus(EStaminaStatus Status) { StaminaStatus = Status; };

	void SetMovementStatus(EMovementStatus Status);

	void SetAttackStatus(EAttackStatus Status);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float StaminaDrainRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MinSprintStamina;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Running")
	float RunningSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly , Category = "Running")
	float SprintingSpeed;

	bool bShiftKeyDown;

	//sprinting start
	void ShiftKeyDown();
	//sprinting stop
	void ShiftKeyUP();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	USphereComponent* RecognitionSphere;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = Camera , meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anims")
	UAnimMontage* BaseMotionMontage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anims")
	UAnimMontage* OneHandMontages;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anims")
	UAnimMontage* DualBladeMontages;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	UParticleSystem* HitParticles;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	UParticleSystem* SkillParticle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	USoundCue* HitSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	USoundCue* SkillSound;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

	float InterpSpeed;

	bool bInterpToEnemy;

	bool bControllerYawLock;

	bool bControllerPitchLock;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category ="Combat")
	float AttackSpeed;

	FORCEINLINE void SetAttackSpeed(float value) { AttackSpeed = value; }
	FORCEINLINE float GetAttackSpeed() { return AttackSpeed; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bHasCombatTarget;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	FVector CombatTargetLocation;

	void SetInterpToEnemy(bool bInterp);

	FRotator GetControllerLookAtYawRotation();

	FRotator GetLookTargetYawRotaion(FVector TargetLocation);

	/*Player Stats*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,Category = "Player Stats")
	float MaxHealth;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Stats")
	float Health;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Stats")
	float MaxStamina;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Stats")
	float Stamina;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Stats")
	int32 Coins;

	UFUNCTION(BlueprintCallable)
	void IncrementCoins(int32 Amount);

	UFUNCTION(BlueprintCallable)
	void IncrementHealth(float Amount);

	void DecrementHealth(float Amount);

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)override;

	void Die();

	UFUNCTION(BlueprintCallable)
	void DeathEnd();

	FTimerHandle DeathTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float DeathDelay;

	TArray<AEnemy*> RecognizedEnemies;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NPC")
	bool b_NearNPC;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NPC")
	ANPC_Character* NearNPC_Character;

	FORCEINLINE void SetNearNPC_Character(ANPC_Character* NearNPC) { NearNPC_Character = NearNPC; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UFUNCTION()
	virtual void RecognitionSphereOnOverlapbegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	UFUNCTION()
	virtual void RecognitionSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void SwitchMovementStatus(EStaminaStatus Status, float DeltaTime);

	void MoveForward(float value);

	void MoveRight(float value);

	void TurnAtRate(float rate);

	void LookUpAtRate(float rate);

	void Charge();
	
	UFUNCTION(BlueprintCallable)
	void OnChargeEnd();

	virtual void Jump() override;

	bool bLMBDown;

	void LMBDown();

	void LMBUp();

	uint8 LMBDownCount;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category = "Combat")
	bool bEquipedWeapon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Items)
	AWeapon* RightEquippedWeapon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Items)
	AWeapon* LeftEquippedWeapon;

	void DetachWeapon();
	void SetEquippedWeapon(AWeapon* Weapon);
	FORCEINLINE AWeapon* GetWEquippedWeapon() { return RightEquippedWeapon; }

	void DeterminWeaponStatus();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Items)
	AItem* ActiveOverlappingItem;

	FORCEINLINE void SetActiveOverlappingItem(AItem* Item) { ActiveOverlappingItem = Item; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anims")
	bool bAttacking;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Anims")
	bool bComboAttack;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Anims")
	bool bStartCheckComboAttack;

	void PlayAttackAnimation(EAttackStatus Attackstatus, EWeaponType WeaponType);

	UFUNCTION(BlueprintCallable)
	void ComboAttackStart();

	UFUNCTION(BlueprintCallable)
	void ComboAttack();

	UFUNCTION(BlueprintCallable)
	void ComboAttackEnd();

	UFUNCTION(BlueprintCallable)
	void PlaySwingSound();

	UFUNCTION(BlueprintCallable)
	void Attack();

	UFUNCTION(BlueprintCallable)
	void AttackEnd();//애님 몽타주에서 불려지고있음.

	UFUNCTION(BlueprintCallable)
	void SkillParticleEmit();

	UFUNCTION(BlueprintCallable)
	void JumpAttackSkill();

	UFUNCTION(BlueprintCallable)
	void SkillEnd();

	UFUNCTION(BlueprintCallable)
	void Disappear();

	void SwitchAttackSequence(EAttackStatus Status, UAnimMontage* MontageSet);

	void DisplayAllEnemyWidgets();

	UFUNCTION(BlueprintCallable)
	void PrepareNearNPC(ANPC_Character* NearNPC, bool b_isNear, ESlateVisibility visibility);

	UFUNCTION(BlueprintCallable)
	void VisibleInfoWidget(ESlateVisibility visibility);

	UFUNCTION(BlueprintCallable)
	void SetInfoText(int32 num);

	UFUNCTION(BlueprintCallable)
	void SetInfoTextByString(FName message);

	UFUNCTION(BlueprintCallable)
	void Interact();

	void AttackReaction();

	void AttackReactionEnd();

	void SetStatusNormal();
};
