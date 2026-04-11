// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA/GA_BuildTower.h"

#include "Object/ZSTower.h"

UGA_BuildTower::UGA_BuildTower()
{
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.BuildTower")));
}

void UGA_BuildTower::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	FVector SpawnLocation = GetAvatarActorFromActorInfo()->GetActorLocation();

	BuildTower(SpawnLocation);
	
	GetWorld()->SpawnActor<AZSTower>(GA_Tower, SpawnLocation, FRotator::ZeroRotator);

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_BuildTower::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_BuildTower::BuildTower(FVector ArgLocation)
{
	GetWorld()->SpawnActor<AZSTower>(GA_Tower, ArgLocation, FRotator::ZeroRotator);

}
