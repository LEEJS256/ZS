// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/ZSProjectile_Chase.h"

AZSProjectile_Chase::AZSProjectile_Chase()
{
}

void AZSProjectile_Chase::Move(float DeltaTime)
{
	if (!LockOnTarget)
		return;

	FVector TargetDir = (LockOnTarget->GetActorLocation() - GetActorLocation()).GetSafeNormal();

	FRotator CurrentRot = GetActorRotation();
	FRotator TargetRot = TargetDir.Rotation();

	FRotator NewRot = FMath::RInterpTo(CurrentRot, TargetRot, DeltaTime, TurnSpeed);

	SetActorRotation(NewRot);

	FVector Forward = GetActorForwardVector();
	SetActorLocation(GetActorLocation() + Forward * ChaseSpeed * DeltaTime);
	
	// FVector CurrentForward = GetActorForwardVector();
	// FVector TargetDir = (LockOnTarget->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	//
	// FVector NewDir = FMath::VInterpNormalRotationTo(CurrentForward, TargetDir, DeltaTime, TurnSpeed);
	//
	// SetActorRotation(NewDir.Rotation());
	// SetActorLocation(GetActorLocation() + NewDir * ChaseSpeed * DeltaTime);
}

void AZSProjectile_Chase::SetTarget(AActor* InTarget)
{
	LockOnTarget = InTarget;
}
