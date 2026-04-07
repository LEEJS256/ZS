// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA/GA_Death.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Character/Monster/ZSMonsterBase.h"

UGA_Death::UGA_Death()
{
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("State.Death")));
	
}

void UGA_Death::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                const FGameplayAbilityActivationInfo ActivationInfo,
                                const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ActorInfo->AvatarActor->DisableInput(nullptr);

	// 몽타주 재생
	PlayMontageAndWait();
}

void UGA_Death::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                           const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
                           bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Death::PlayMontageAndWait()
{
	FName SectionName = FName("Default");
	
	UAbilityTask_PlayMontageAndWait* Task =
		UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,
			NAME_None,
			DeathMontages[0],
			1.0f,
			SectionName
		);

	Task->OnCompleted.AddDynamic(this, &UGA_Death::OnDeathEnd);
	Task->OnInterrupted.AddDynamic(this, &UGA_Death::OnDeathEnd);
	
	Task->ReadyForActivation();
}

void UGA_Death::OnDeathEnd()
{
	AActor* Avatar = GetAvatarActorFromActorInfo();
	
	if (Avatar)
	{
		AZSMonsterBase* Monster = Cast<AZSMonsterBase>(Avatar);

		if (Monster)
		{
			Monster->DeathMonster();
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Monster Cast Failed"));
		}
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
