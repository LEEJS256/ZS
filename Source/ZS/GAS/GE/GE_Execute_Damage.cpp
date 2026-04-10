// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GE/GE_Execute_Damage.h"

#include "GAS/Attribute/ZSAttributeSet.h"

UGE_Execute_Damage::UGE_Execute_Damage()
{
	// RelevantAttributesToCapture.Add(DamageCaptureDefs.AttackDef);
	// RelevantAttributesToCapture.Add(DamageCaptureDefs.ArmorDef);
}

void UGE_Execute_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
                                                FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	// 🔹 ASC 가져오기
	UAbilitySystemComponent* SourceASC =
		Spec.GetContext().GetOriginalInstigatorAbilitySystemComponent();

	UAbilitySystemComponent* TargetASC =
		ExecutionParams.GetTargetAbilitySystemComponent();

	if (!TargetASC)
		return;

	// 🔹 Attribute 값
	float Attack = 0.f;
	float Defense = 0.f;
	float TargetHealth = 0.f;

	// 🔹 Source (가해자)
	if (SourceASC)
	{
		if (const UZSAttributeSet* SourceSet = SourceASC->GetSet<UZSAttributeSet>())
		{
			Attack = SourceSet->GetAttack();
		}
	}

	// 🔹 Target (피해자)
	if (TargetASC)
	{
		if (const UZSAttributeSet* TargetSet = TargetASC->GetSet<UZSAttributeSet>())
		{
			Defense = TargetSet->GetArmor();
			TargetHealth = TargetSet->GetHealth();

			// 이미 죽은 상태면 계산 안함
			if (TargetHealth <= 0.f)
				return;
		}
	}

	// 🔹 기본 보정
	Attack = FMath::Max(Attack, 0.f);
	Defense = FMath::Max(Defense, 0.f);

	// 🔹 SetByCaller (스킬 계수)
	float SkillMultiplier = Spec.GetSetByCallerMagnitude(
		FGameplayTag::RequestGameplayTag("Data.DamageMultiplier"),
		false,
		1.0f
	);

	// 🔹 방어 공식
	const float ArmorFactor = 0.5f;
	float DamageMultiplier = 1.f / (1.f + Defense * ArmorFactor);

	// 🔹 최종 데미지
	float FinalDamage = Attack * SkillMultiplier * DamageMultiplier * DamageCoefficient;
	FinalDamage = FMath::Max(FinalDamage, 0.f);

	// 🔹 적용 (체력 감소)
	OutExecutionOutput.AddOutputModifier(
		FGameplayModifierEvaluatedData(
			UZSAttributeSet::GetHealthAttribute(),
			EGameplayModOp::Additive,
			-FinalDamage
		)
	);
}
