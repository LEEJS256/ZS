// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotify/ZS_AnimNotify_Bomb.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Character/ZSPlayerCharacter.h"
#include "PlayerState/ZSPlayerState.h"

UZS_AnimNotify_Bomb::UZS_AnimNotify_Bomb()
{
}

void UZS_AnimNotify_Bomb::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	AZSPlayerCharacter* PlayerCharacter = Cast<AZSPlayerCharacter>(Owner);
	if (!PlayerCharacter) return;

	AZSPlayerState* PS = PlayerCharacter->GetPlayerState<AZSPlayerState>();
	if (!PS)
		return;
	FGameplayEventData EventData;
	EventData.EventTag = EventTag;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		PS,
		EventTag,
		EventData
	);
	
}
