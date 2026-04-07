// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Attribute/ZSAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectTypes.h"   // FGameplayEffectModCallbackData 포함
#include "GameplayEffectExtension.h" // 일부 확장 관련 기능

UZSAttributeSet::UZSAttributeSet()
{
	InitHealth(180.f);
	InitMaxHealth(200.f);
	InitStamina(50.f);
	InitMaxStamina(100.f);
	InitAttack(10.f);
	InitArmor(5.f);
	InitPoise(20.f);
	InitExp(10.f);
	InitLevel(1.f);
	InitGold(100.f);
	InitSpeed(500.f);
	InitSprintWeight(1.3f);
}

void UZSAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UZSAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UZSAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UZSAttributeSet, Stamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UZSAttributeSet, MaxStamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UZSAttributeSet, Gold, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UZSAttributeSet, Exp, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UZSAttributeSet, Level, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UZSAttributeSet, Attack, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UZSAttributeSet, Armor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UZSAttributeSet, Poise, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UZSAttributeSet, Speed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UZSAttributeSet, SprintWeight, COND_None, REPNOTIFY_Always);

}

void UZSAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetHealthAttribute())
	{
		//MaxHealth 넘지않게 하기
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}
	
	//스태미나 
	if (Attribute == GetStaminaAttribute())
	{
		//MaxStatmina 넘지않게 하기
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxStamina());
	}
	
}

void UZSAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);
}

void UZSAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

#pragma region Stat
	//Health
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		const float Delta = Data.EvaluatedData.Magnitude;

		if (Delta < 0.f)
		{
		//	OnDamageTaken.Broadcast(-Delta);
		}

		//최종체력
 		float NewHealth = GetHealth();
		
		if (NewHealth <= 0.f)
		{
			// AActor* Owner = GetOwningActor();
			UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();

			if (ASC)
			{
				ASC->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag("State.Death"));
			}
		}
	}
	else if (Data.EvaluatedData.Attribute == GetMaxHealthAttribute())
	{
		const float DeltaValue = Data.EvaluatedData.Magnitude;

		// 현재 체력에 변화량만큼 더하기
		SetHealth(FMath::Clamp(GetHealth() + DeltaValue, 0.0f, GetMaxHealth()));
	}

	//Stamina

	if (Data.EvaluatedData.Attribute == GetStaminaAttribute())
	{
		SetStamina(FMath::Clamp(GetStamina(), 0.0f, GetMaxStamina()));
	}
	else if (Data.EvaluatedData.Attribute == GetMaxStaminaAttribute())
	{
		//변화량
		const float DeltaValue = Data.EvaluatedData.Magnitude;

		SetStamina(FMath::Clamp(GetStamina() + DeltaValue, 0.0f, GetMaxStamina()));
	}
#pragma endregion
}
	


void UZSAttributeSet::OnRep_Speed(const FGameplayAttributeData& OldSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UZSAttributeSet, Speed, OldSpeed);
}

void UZSAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UZSAttributeSet, Health, OldHealth);
}

void UZSAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UZSAttributeSet, MaxHealth, OldMaxHealth);
}

void UZSAttributeSet::OnRep_Stamina(const FGameplayAttributeData& OldStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UZSAttributeSet, Stamina, OldStamina);
}

void UZSAttributeSet::OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UZSAttributeSet, MaxStamina, OldMaxStamina);
}

void UZSAttributeSet::OnRep_Exp(const FGameplayAttributeData& OldExp)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UZSAttributeSet, Exp, OldExp);
}

void UZSAttributeSet::OnRep_Level(const FGameplayAttributeData& OldLevel)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UZSAttributeSet, Level, OldLevel);
}

void UZSAttributeSet::OnRep_Gold(const FGameplayAttributeData& OldGold)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UZSAttributeSet, Gold, OldGold);
}

void UZSAttributeSet::OnRep_Attack(const FGameplayAttributeData& OldAttack)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UZSAttributeSet, Attack, OldAttack);
}

void UZSAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldArmor)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UZSAttributeSet, Armor, OldArmor);
}

void UZSAttributeSet::OnRep_Poise(const FGameplayAttributeData& OldPoise)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UZSAttributeSet, Poise, OldPoise);
}

void UZSAttributeSet::OnRep_SprintWeight(const FGameplayAttributeData& OldSprintWeight)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UZSAttributeSet, SprintWeight, OldSprintWeight);
}
