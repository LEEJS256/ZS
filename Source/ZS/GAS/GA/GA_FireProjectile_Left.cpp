// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA/GA_FireProjectile_Left.h"

#include "Character/ZSPlayerCharacter.h"
#include "Object/ZSProjectile.h"

UGA_FireProjectile_Left::UGA_FireProjectile_Left()
{
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("ATK.Left")));
}

void UGA_FireProjectile_Left::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                              const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                              const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!ActorInfo || !ActorInfo->AvatarActor.IsValid())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	FireProjectile();

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UGA_FireProjectile_Left::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_FireProjectile_Left::FireProjectile()
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!IsValid(AvatarActor))
		return;

	AZSPlayerCharacter* PlayerCharacter = Cast<AZSPlayerCharacter>(AvatarActor);
	if (!IsValid(PlayerCharacter))
		return;

	if (!GAProjectile)
		return;

	// 멀티플레이 기준이면 서버에서만 생성
	if (!AvatarActor->HasAuthority())
		return;

	const FVector SpawnLocation = GetSpawnLocation();
	const FRotator SpawnRotation = GetSpawnRotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = PlayerCharacter;
	SpawnParams.Instigator = PlayerCharacter;

	AActor* SpawnedProjectile = GetWorld()->SpawnActor<AActor>(
		GAProjectile,
		SpawnLocation,
		SpawnRotation,
		SpawnParams
	);
}

FVector UGA_FireProjectile_Left::GetSpawnLocation() const
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	AZSPlayerCharacter* PlayerCharacter = Cast<AZSPlayerCharacter>(AvatarActor);
	if (!IsValid(PlayerCharacter))
		return FVector::ZeroVector;

	USkeletalMeshComponent* MeshComp = PlayerCharacter->GetMesh();
	FVector SpawnLocation =
		PlayerCharacter->GetActorLocation()
		+ PlayerCharacter->GetActorForwardVector() * SpawnOffset.X
		+ PlayerCharacter->GetActorRightVector() * SpawnOffset.Y
		+ PlayerCharacter->GetActorUpVector() * SpawnOffset.Z;
	return SpawnLocation;
}

FRotator UGA_FireProjectile_Left::GetSpawnRotation() const
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!IsValid(AvatarActor))
		return FRotator::ZeroRotator;

	FRotator ActorRot = AvatarActor->GetActorRotation();

	// Pitch, Roll 제거 (핵심)
	return FRotator(0.f, ActorRot.Yaw, 0.f);
}
