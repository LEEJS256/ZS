// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA/GA_Right_ATK.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Character/ZSPlayerCharacter.h"
#include "Object/ZS_bomb.h"
#include "PlayerState/ZSPlayerState.h"
#include "Utility/ZSNativeGameplayTag.h"

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

	if (!AttackMontages.IsValidIndex(0) || !AttackMontages[0])
	{
		UE_LOG(LogTemp, Error, TEXT("Montage NULL"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Montage Play Try: %s"), *GetNameSafe(AttackMontages[0]));
	PlayTask->ReadyForActivation();

	EventTagTask =
		UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
			this,
			FGameplayTag::RequestGameplayTag("Event.Bomb.Active"),
			// nullptr,
			ActorInfo->OwnerActor.Get(),
			false,
			false 
		);

	EventTagTask->EventReceived.AddDynamic(this, &UGA_Right_ATK::OnFireEvent);
	EventTagTask->ReadyForActivation();

	AZSPlayerCharacter* Character = Cast<AZSPlayerCharacter>(GetAvatarActorFromActorInfo());
	AZSPlayerState* PS = Character->GetPlayerState<AZSPlayerState>();
	if (PS)
	{
		PS->GrantStateTag(TAG_State_ATK);
	}
}

void UGA_Right_ATK::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                               const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
                               bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	AZSPlayerCharacter* Character = Cast<AZSPlayerCharacter>(GetAvatarActorFromActorInfo());
	AZSPlayerState* PS = Character->GetPlayerState<AZSPlayerState>();
	if (PS)
	{
		PS->GrantStateTag(TAG_State_Idle);
	}
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

	FVector Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector();
	FVector TargetPos = SpawnLocation + (Forward * ProjectileRange); // 
	
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
	if (!ASC)
		return;

	FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
	Context.AddSourceObject(this);

	// Projectile->Set_GE(DamageEffectClass,Context);
	Projectile->InitProjectile(SpawnLocation,TargetPos,ThrowAngle);
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

	//카메라 위치에서 카메라방향으로 10000유닛쏴서 끝지점 생성
	FVector TraceEnd = CameraLoc + (CameraRot.Vector() * 10000.f);

	//라인트레이스 준비
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

	//맞았으면 맞은 위치, 안맞았으면 그저 먼지지점
	FVector TargetLocation = bHit ? Hit.ImpactPoint : TraceEnd;

	FVector MuzzleLocation = GetSpawnLocation();

	return (TargetLocation - MuzzleLocation).Rotation();
}

FVector UGA_Right_ATK::GetTargetLocation() const
{
	FVector TargetLoca ;

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC)
		return TargetLoca;

	FVector CameraLoc;
	FRotator CameraRot;
	PC->GetPlayerViewPoint(CameraLoc, CameraRot);

	//카메라 위치에서 카메라방향으로 10000유닛쏴서 끝지점 생성
	FVector TraceEnd = CameraLoc + (CameraRot.Vector() * 10000.f);

	//라인트레이스 준비
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

	TargetLoca = bHit ? Hit.ImpactPoint : TraceEnd;

	return TargetLoca;
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

	if (EventTagTask)
	{
		EventTagTask->EndTask(); // 🔥 이벤트 더 이상 못 받음
	}
}
