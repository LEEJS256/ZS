// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/GA/ZSGameplayAbility.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "GA_Death.generated.h"

/**
 * 
 */
UCLASS()
class ZS_API UGA_Death : public UZSGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_Death();

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
	void PlayMontageAndWait();

	UFUNCTION()
	void OnDeathEnd();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Weapon|Animation|Attack")
	TArray<TObjectPtr<UAnimMontage>> DeathMontages;
	
};
