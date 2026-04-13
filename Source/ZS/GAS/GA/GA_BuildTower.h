// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/GA/ZSGameplayAbility.h"
#include "Object/ZSTower.h"
#include "GA_BuildTower.generated.h"

/**
 * 
 */

class AZSTower_Preview;

UCLASS()
class ZS_API UGA_BuildTower : public UZSGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_BuildTower();

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
	UFUNCTION()
	void OnRightClick(FGameplayEventData Payload);
	UFUNCTION()
	void OnLeftClick(FGameplayEventData Payload);

	UPROPERTY()
	class UAbilityTask_WaitGameplayEvent* RightClickTask;
	UPROPERTY()
	class UAbilityTask_WaitGameplayEvent* LeftClickTask;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Projectile")
	TSubclassOf<AZSTower> GA_Tower;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Projectile")
	TSubclassOf<AZSTower_Preview> GA_Tower_Preview;

	UPROPERTY()
	AZSTower_Preview* Preview_Tower;
	void BuildTower(FVector ArgLocation);
};
