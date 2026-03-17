// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "ZSGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class ZS_API UZSGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
public:
	UZSGameplayAbility();

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
		bool bWasCancelled
	) override;

	virtual void ApplyDamageFromTrace();

	
	virtual void OnStopAttackTrace_Server();

protected:
	/** Damage Spec 생성 시 공격 타입 태그를 넣어준다 */
	void AddAttackTypeToEffectSpec(FGameplayEffectSpec& Spec) const;

	// void ApplyMotionWarp(AZSPlayerCharacter* SKPlayer);
	// FVector GetAttackTargetLocation() const;
	
	UPROPERTY(EditAnywhere, Category = "SK|GAS")
	TArray<TSubclassOf<UGameplayEffect>> HeatGE;

	void ApplyHeatGE(int32 HeatIndex,UAbilitySystemComponent* SourceASC);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attack")
	FGameplayTag AttackTypeTag;
};
