// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA/GA_BuildTower.h"

#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Object/ZSTower.h"
#include "Object/ZSTower_Preview.h"
#include "Utility/ZSNativeGameplayTag.h"

UGA_BuildTower::UGA_BuildTower()
{
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.BuildTower")));
}

void UGA_BuildTower::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
#pragma  region Preview
	
	// 좌클릭 대기
	LeftClickTask =
		UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, TAG_Input_LeftClick);

	LeftClickTask->EventReceived.AddDynamic(this, &UGA_BuildTower::OnLeftClick);
	LeftClickTask->ReadyForActivation();

	// 우클릭 대기
	RightClickTask =
		UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, TAG_Input_RightClick);

	RightClickTask->EventReceived.AddDynamic(this, &UGA_BuildTower::OnRightClick);
	RightClickTask->ReadyForActivation();

	Preview_Tower = GetWorld()->SpawnActor<AZSTower_Preview>(GA_Tower_Preview);
#pragma endregion 

}

void UGA_BuildTower::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_BuildTower::OnRightClick(FGameplayEventData Payload)
{
	if (Preview_Tower)
	{
		Preview_Tower->Destroy();
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UGA_BuildTower::OnLeftClick(FGameplayEventData Payload)
{
	if (!Preview_Tower)
		return;

	if (Preview_Tower->GetValidPosition())
	{
		GetWorld()->SpawnActor<AZSTower>(GA_Tower, Preview_Tower->GetActorTransform());
		Preview_Tower->Destroy();

		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}

void UGA_BuildTower::BuildTower(FVector ArgLocation)
{
	GetWorld()->SpawnActor<AZSTower>(GA_Tower, ArgLocation, FRotator::ZeroRotator);

}
