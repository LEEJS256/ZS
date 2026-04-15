// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/ZSProjectile.h"
#include "ZSProjectile_Chase.generated.h"

/**
 * 
 */
UCLASS()
class ZS_API AZSProjectile_Chase : public AZSProjectile
{
	GENERATED_BODY()
public:
	AZSProjectile_Chase();

	virtual void Move(float DeltaTime);

	void SetTarget(AActor* InTarget);

protected:
	UPROPERTY()
	AActor* LockOnTarget;

	UPROPERTY(EditAnywhere)
	float ChaseSpeed = 1000.f;

	UPROPERTY(EditAnywhere)
	float TurnSpeed = 40.f; 
};
