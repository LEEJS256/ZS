// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotify/ZS_AnimNofify_FireProjectile.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "Character/ZSPlayerCharacter.h"
#include "PlayerState/ZSPlayerState.h"

UZS_AnimNofify_FireProjectile::UZS_AnimNofify_FireProjectile()
{
}

void UZS_AnimNofify_FireProjectile::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	if (!MeshComp) return;

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	AZSPlayerCharacter* PlayerCharacter = Cast<AZSPlayerCharacter>(Owner);
	if (!PlayerCharacter) return;

	AZSPlayerState* PS = PlayerCharacter->GetPlayerState<AZSPlayerState>();
	if (!PS) return;
	FGameplayEventData EventData;
	EventData.EventTag = EventTag;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		PS,
		EventTag,
		EventData
	);

}
