// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "Enemy_Boss.generated.h"

UENUM(BlueprintType)
enum class EBossAttackStatus : uint8
{
	EAS_Idle,
	EAS_Attack1,
	EAS_Attack2,
	EAS_Attack3,
	EAS_Skill,

	EAS_MAX,
};

class AAIController;
class USphereComponent;
class AMain;
class UParticleSystem;
class AWeapon;
class USoundCue;
class UBoxComponent;
class UAnimMontage;
class UEnemyHealthBar;
class UStaticMeshComponent;
UCLASS()
class ONLINELEARNIG_4_24_3_API AEnemy_Boss : public AEnemy
{
	GENERATED_BODY()
public:
	AEnemy_Boss();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	EBossAttackStatus EnemyAttackStatus;

	FORCEINLINE void SetAttackStatus(EBossAttackStatus Status) { EnemyAttackStatus = Status; }

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	UBoxComponent* LeftHandCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	UBoxComponent* RightHandCollision;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Combat")
	TSubclassOf<AActor> RightHandWeapon;

protected:
	virtual void DamageReact_Anim(float In_Damage) override;

	virtual void AttackMotion_Anim() override;

	virtual void BeginPlay() override;
public:

	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void RightOnOverlapbegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	UFUNCTION()
	void RightOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void LeftOnOverlapbegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	UFUNCTION()
	void LeftOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable)
	void Attack_Boss();

	UFUNCTION(BlueprintCallable)
	void AttackEnd_Boss();

	UFUNCTION(BlueprintCallable)
	void ActivateRightCollision();

	UFUNCTION(BlueprintCallable)
	void ActivateLeftCollision();

	UFUNCTION(BlueprintCallable)
	void DeactivateCollision_Boss();

	UFUNCTION(BlueprintCallable)
	void BossReactionEnd();

	UFUNCTION(BlueprintCallable)
	void SkillEnd();

	UFUNCTION()
	virtual void DamageProcess(AActor* OtherActor) override;

	virtual void Die();
	 
	virtual void Disappear() override;
private:
	
	int32 NormalAttackCount;
};
