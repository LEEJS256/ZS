// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA/GA_FireProjectile_Left.h"

#include "Character/ZSPlayerCharacter.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Object/ZSProjectile.h"
#include "Utility/ZSNativeGameplayTag.h"

UGA_FireProjectile_Left::UGA_FireProjectile_Left()
{
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("ATK.Left")));
}

void UGA_FireProjectile_Left::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                              const FGameplayAbilityActorInfo* ActorInfo,
                                              const FGameplayAbilityActivationInfo ActivationInfo,
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

	EventTagTask =
		UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
			this,
			FGameplayTag::RequestGameplayTag("Event.Fire"),
			nullptr,
			false,
			false //onlymatchingExact 끄기 부모태그도 가능하도록
		);

	EventTagTask->EventReceived.AddDynamic(this, &UGA_FireProjectile_Left::OnFireEvent);
	EventTagTask->ReadyForActivation();
	// EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	
	
	FName SectionName = FName("Default");

	UAbilityTask_PlayMontageAndWait* PlayTask =
		UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,
			NAME_None,
			AttackMontages[0],
			1.0f,
			SectionName
		);

	PlayTask->OnCompleted.AddDynamic(this, &UGA_FireProjectile_Left::OnMontageCompleted);
	PlayTask->OnInterrupted.AddDynamic(this, &UGA_FireProjectile_Left::OnMontageInterrupted);
	PlayTask->OnCancelled.AddDynamic(this, &UGA_FireProjectile_Left::OnMontageInterrupted);

	//사용 O
	PlayTask->ReadyForActivation();
}

void UGA_FireProjectile_Left::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                         const FGameplayAbilityActorInfo* ActorInfo,
                                         const FGameplayAbilityActivationInfo ActivationInfo,
                                         bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_FireProjectile_Left::FireProjectile(FGameplayTag ParaTag)
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

	const FVector SpawnLocation = GetSpawnLocation(ParaTag);
	const FRotator SpawnRotation = GetSpawnRotationFromCrossHair(ParaTag);

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

FVector UGA_FireProjectile_Left::GetSpawnLocation(FGameplayTag ParaTag) const
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	AZSPlayerCharacter* PlayerCharacter = Cast<AZSPlayerCharacter>(AvatarActor);
	FVector SpawnLocation = FVector::ZeroVector;
	if (!IsValid(PlayerCharacter))
		return SpawnLocation;

	USkeletalMeshComponent* MeshComp = PlayerCharacter->GetMesh();
	if (ParaTag == TAG_Event_Fire_Left)
	{
		SpawnLocation = MeshComp->GetSocketLocation("weapon_Fire_Left");
		// FRotator SpawnRotation = MeshComp->GetSocketRotation("Muzzle");
	}
	else
	{
		SpawnLocation = MeshComp->GetSocketLocation("weapon_Fire_Right");
	}
	// FVector SpawnLocation =
	// 	PlayerCharacter->GetActorLocation()
	// 	+ PlayerCharacter->GetActorForwardVector() * SpawnOffset.X
	// 	+ PlayerCharacter->GetActorRightVector() * SpawnOffset.Y
	// 	+ PlayerCharacter->GetActorUpVector() * SpawnOffset.Z;
	return SpawnLocation;
}

FRotator UGA_FireProjectile_Left::GetSpawnRotation(int32 LeftRightNum) const
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!IsValid(AvatarActor))
		return FRotator::ZeroRotator;
	
	const FGameplayAbilityActorInfo* ActorInfo = GetCurrentActorInfo();
	if (ActorInfo->PlayerController.IsValid())
	{
		FRotator ControlRot = ActorInfo->PlayerController->GetControlRotation();

		// Pitch, Roll 제거 (원하면 유지 가능)
		return FRotator(0.f, ControlRot.Yaw, 0.f);
	}

	return FRotator::ZeroRotator;
	
	// FRotator ActorRot = AvatarActor->GetActorRotation();
	//
	// // Pitch, Roll 제거 (핵심)
	// return FRotator(0.f, ActorRot.Yaw, 0.f);
}

FRotator UGA_FireProjectile_Left::GetSpawnRotationFromCrossHair(FGameplayTag ParaTag)
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC) return FRotator::ZeroRotator;

	FVector CameraLoc;
	FRotator CameraRot;
	PC->GetPlayerViewPoint(CameraLoc, CameraRot);

	FVector TraceEnd = CameraLoc + (CameraRot.Vector() * 10000.f);

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetAvatarActorFromActorInfo());

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		Hit,
		CameraLoc,
		TraceEnd,
		ECC_Visibility,
		Params
	);

	FVector TargetLocation = bHit ? Hit.ImpactPoint : TraceEnd;

	FVector MuzzleLocation = GetSpawnLocation(ParaTag);

	return (TargetLocation - MuzzleLocation).Rotation();
}

void UGA_FireProjectile_Left::OnMontageCompleted()
{
	EndAbility(
		CurrentSpecHandle,
		CurrentActorInfo,
		CurrentActivationInfo,
		false,
		false
	);
}

void UGA_FireProjectile_Left::OnMontageInterrupted()
{
	EndAbility(
		CurrentSpecHandle,
		CurrentActorInfo,
		CurrentActivationInfo,
		false,
		true
	);
}

void UGA_FireProjectile_Left::OnFireEvent(FGameplayEventData Payload)
{
	FireProjectile(Payload.EventTag);
}
