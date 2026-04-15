// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/GA/ZSGameplayAbility.h"
#include "GA_Tower_ATK.generated.h"

/**
 * 
 */
class AZSProjectile;
UCLASS()
class ZS_API UGA_Tower_ATK : public UZSGameplayAbility
{
	GENERATED_BODY()
public:
	UGA_Tower_ATK();

	
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled) override;
protected:
	void FireProjectile();

	TArray<FVector> GetSpawnLocationArray(); 
	FRotator GetSpawnRotation(int32 LeftRightNum = 1) const;
	void SpawnProjectile(FVector Location,FRotator Rotator,FActorSpawnParameters Param,AActor* Target);
		

	UPROPERTY(EditAnywhere)
	TArray<FName> MuzzleSockets;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Projectile")
	TSubclassOf<AZSProjectile> GAProjectile;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Damage")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	
};
