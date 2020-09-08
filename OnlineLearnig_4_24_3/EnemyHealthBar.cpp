// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyHealthBar.h"



void UEnemyHealthBar::SetHealth(float Value)
{
	Health = Value;
}

void UEnemyHealthBar::SetMaxHealth(float Value)
{
	MaxHealth = Value;
}

void UEnemyHealthBar::NativeConstruct()
{
	Health = 0.f;

	MaxHealth = 100.f;
}
