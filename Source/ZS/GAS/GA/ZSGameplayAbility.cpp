// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA/ZSGameplayAbility.h"

#include "AbilitySystemComponent.h"

UZSGameplayAbility::UZSGameplayAbility()
{
}

void UZSGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                         const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                         const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UZSGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UZSGameplayAbility::ApplyDamageFromTrace()
{
}

void UZSGameplayAbility::OnStopAttackTrace_Server()
{
	//몽타주 1번에 여러 공격이 들어갈 때 데미지 빨리 처리 시 불리는 함수
	ApplyDamageFromTrace();
}

void UZSGameplayAbility::AddAttackTypeToEffectSpec(FGameplayEffectSpec& Spec) const
{
	if (AttackTypeTag.IsValid())
	{
		Spec.DynamicGrantedTags.AddTag(AttackTypeTag);
	}
}

void UZSGameplayAbility::ApplyHeatGE(int32 HeatIndex, UAbilitySystemComponent* SourceASC)
{
	TSubclassOf<UGameplayEffect> HeatEffectClass =
	HeatGE[0];

	if (HeatEffectClass)
	{
		FGameplayEffectContextHandle HeatContext =
			SourceASC->MakeEffectContext();

		HeatContext.AddSourceObject(this);

		FGameplayEffectSpecHandle HeatSpecHandle =
			SourceASC->MakeOutgoingSpec(HeatEffectClass, 1.f, HeatContext);

		if (HeatSpecHandle.IsValid())
		{
			SourceASC->ApplyGameplayEffectSpecToSelf(
				*HeatSpecHandle.Data.Get()
			);
		}
	}
}
