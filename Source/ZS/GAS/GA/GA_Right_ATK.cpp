// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA/GA_Right_ATK.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Character/ZSPlayerCharacter.h"
#include "Object/ZS_bomb.h"

UGA_Right_ATK::UGA_Right_ATK()
{
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("ATK.Right")));
}

void UGA_Right_ATK::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
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
			FGameplayTag::RequestGameplayTag("Event.Bomb"),
			nullptr,
			false,
			false //onlymatchingExact 끄기 부모태그도 가능하도록
		);

	EventTagTask->EventReceived.AddDynamic(this, &UGA_Right_ATK::OnFireEvent);
	EventTagTask->ReadyForActivation();
	
	FName SectionName = FName("Default");

	UAbilityTask_PlayMontageAndWait* PlayTask =
		UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,
			NAME_None,
			AttackMontages[0],
			1.0f,
			SectionName
		);

	PlayTask->OnCompleted.AddDynamic(this, &UGA_Right_ATK::OnMontageCompleted);
	PlayTask->OnInterrupted.AddDynamic(this, &UGA_Right_ATK::OnMontageInterrupted);
	PlayTask->OnCancelled.AddDynamic(this, &UGA_Right_ATK::OnMontageInterrupted);

	//사용 O
	PlayTask->ReadyForActivation();
}

void UGA_Right_ATK::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                               const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
                               bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Right_ATK::FireProjectile()
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
	const FRotator SpawnRotation = GetSpawnRotationFromCrossHair();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = PlayerCharacter;
	SpawnParams.Instigator = PlayerCharacter;

	AActor* SpawnedProjectile = GetWorld()->SpawnActor<AActor>(
		GAProjectile,
		SpawnLocation,
		SpawnRotation,
		SpawnParams
	);

	AZS_bomb* Projectile = Cast<AZS_bomb>(SpawnedProjectile);
	if (!Projectile)
		return;

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC) return;

	FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
	Context.AddSourceObject(this);

	// Projectile->Set_GE(DamageEffectClass,Context);
}

FVector UGA_Right_ATK::GetSpawnLocation() const
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	AZSPlayerCharacter* PlayerCharacter = Cast<AZSPlayerCharacter>(AvatarActor);
	FVector SpawnLocation = FVector::ZeroVector;
	if (!IsValid(PlayerCharacter))
		return SpawnLocation;

	USkeletalMeshComponent* MeshComp = PlayerCharacter->GetMesh();

	SpawnLocation = MeshComp->GetSocketLocation(RightHand_Socket[0]);

	return SpawnLocation;
}

FRotator UGA_Right_ATK::GetSpawnRotationFromCrossHair()
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

	FVector MuzzleLocation = GetSpawnLocation();

	return (TargetLocation - MuzzleLocation).Rotation();
}

void UGA_Right_ATK::OnMontageCompleted()
{
	EndAbility(
		CurrentSpecHandle,
		CurrentActorInfo,
		CurrentActivationInfo,
		false,
		false
	);
}

void UGA_Right_ATK::OnMontageInterrupted()
{
	EndAbility(
		CurrentSpecHandle,
		CurrentActorInfo,
		CurrentActivationInfo,
		false,
		true
	);
}

void UGA_Right_ATK::OnFireEvent(FGameplayEventData Payload)
{
	FireProjectile();
}
