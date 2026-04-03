// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/GA/ZSGameplayAbility.h"
#include "Object/ZSProjectile.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Utility/ZSNativeGameplayTag.h"
#include "GA_FireProjectile_Left.generated.h"

/**
 * 
 */
UCLASS()
class ZS_API UGA_FireProjectile_Left : public UZSGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_FireProjectile_Left();

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
	void FireProjectile(FGameplayTag ParaTag = TAG_Event_Fire_Left);
	FVector GetSpawnLocation(FGameplayTag ParaTag = TAG_Event_Fire_Left) const;
	FRotator GetSpawnRotation(int32 LeftRightNum = 1) const;
	FRotator GetSpawnRotationFromCrossHair(FGameplayTag ParaTag);
protected:

	UPROPERTY()
	UAbilityTask_WaitGameplayEvent* EventTagTask;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Weapon|Animation|Attack")
	TArray<TObjectPtr<UAnimMontage>> AttackMontages;
		
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Projectile")
	TSubclassOf<AZSProjectile> GAProjectile;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Projectile")
	FVector SpawnOffset = FVector(230.f, 100.f, 100.f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Projectile")
	float MaxRange = 3000.f;

	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Animation")
	// TObjectPtr<UAnimMontage> FireMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Damage")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Damage")
	float BaseDamage = 10.f;

	UFUNCTION()
	void OnMontageCompleted();

	UFUNCTION()
	void OnMontageInterrupted();

	UFUNCTION()
	void OnFireEvent(FGameplayEventData Payload);
};
