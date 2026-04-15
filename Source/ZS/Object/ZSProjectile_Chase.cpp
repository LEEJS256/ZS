// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/ZSProjectile_Chase.h"

#include "GameFramework/ProjectileMovementComponent.h"

AZSProjectile_Chase::AZSProjectile_Chase()
{
	ProjectileMovement->bIsHomingProjectile = true;
	// ProjectileMovement->HomingTargetComponent = LockOnTarget->GetRootComponent();
	ProjectileMovement->HomingAccelerationMagnitude = 10000.f; // 중요
}

void AZSProjectile_Chase::Move(float DeltaTime)
{
	if (!LockOnTarget)
		return;
}

void AZSProjectile_Chase::SetTarget(AActor* InTarget)
{
	LockOnTarget = InTarget;
	
	ProjectileMovement->HomingTargetComponent = LockOnTarget->GetRootComponent();
}
