// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA/GA_Sprint.h"

#include "Character/ZSPlayerCharacter.h"
#include "GAS/Attribute/ZSAttributeSet.h"

UGA_Sprint::UGA_Sprint()
{
}

void UGA_Sprint::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                 const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	AZSPlayerCharacter* Character = Cast<AZSPlayerCharacter>(ActorInfo->AvatarActor.Get());
	if (!Character)
		return;
	
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo)) 
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	UE_LOG(LogTemp, Error, TEXT(">>> SPRINT GA ACTIVATED <<<"));
	Character->SetSprinting(true);
	ConsumeStamina();

	// 스태미나 소비용 타이머 시작
	GetWorld()->GetTimerManager().SetTimer(StaminaTimerHandle, this, &UGA_Sprint::ConsumeStamina, 0.1f, true);
}


void UGA_Sprint::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                            const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	EndSprint();
}

void UGA_Sprint::ConsumeStamina()
{
	AZSPlayerCharacter* Character = Cast<AZSPlayerCharacter>(GetAvatarActorFromActorInfo());
	if (!Character)
		return;

	UAbilitySystemComponent* ASC = Character->GetAbilitySystemComponent();
	if (!ASC)
		return;

	// GE 클래스 지정
	if (!SprintStaminaCostEffect)
		return;

	// 스태미나가 0 이하라면 종료
	const float CurrentStamina = ASC->GetNumericAttribute(UZSAttributeSet::GetStaminaAttribute());
	if (CurrentStamina <= 0.f)
	{
		EndSprint();
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	// 스태미나 코스트 GE 적용
	FGameplayEffectSpecHandle SpecHandle = ASC->
		MakeOutgoingSpec(SprintStaminaCostEffect, 1.f, ASC->MakeEffectContext());
	if (SpecHandle.IsValid())
	{
		ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}

void UGA_Sprint::EndSprint()
{
	GetWorld()->GetTimerManager().ClearTimer(StaminaTimerHandle);


	AZSPlayerCharacter* Character = Cast<AZSPlayerCharacter>(GetAvatarActorFromActorInfo());
	if (Character)
	{
		Character->SetSprinting(false);
	}
}
