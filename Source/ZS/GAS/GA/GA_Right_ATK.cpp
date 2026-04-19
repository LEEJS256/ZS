// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA/GA_Right_ATK.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Character/ZSPlayerCharacter.h"
#include "Component/ZSPreviewComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Object/ZS_bomb.h"
#include "PlayerState/ZSPlayerState.h"
#include "Utility/ZSNativeGameplayTag.h"

UGA_Right_ATK::UGA_Right_ATK()
{
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("ATK.Right")));
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	bRetriggerInstancedAbility = true;
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
	ActivationOwnedTags.AddTag(TAG_State_ATK);

	if (!bIsAiming)
	{
		bIsAiming = true;
		
		AZSPlayerCharacter* Character = Cast<AZSPlayerCharacter>(GetAvatarActorFromActorInfo());
		if (Character && Character->PreviewComponent)
		{
			// FTrajectoryParams Params;
			// Params.StartLocation = GetSpawnLocation();
			// Params.LaunchVelocity = (GetTargetLocation() - Params.StartLocation).GetSafeNormal() * ProjectileSpeed;
			// Params.Radius = 5.f;
			
			Character->PreviewComponent->StartPreview_RightGA(this);
		}

		return;
		
	}
	else
	{
		bIsAiming = false;
		PlayMontage();
		
		AZSPlayerCharacter* Character = Cast<AZSPlayerCharacter>(GetAvatarActorFromActorInfo());
		if (Character && Character->PreviewComponent)
		{
			Character->PreviewComponent->StopPreview();
		}

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
	}
}

void UGA_Right_ATK::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                               const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
                               bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	ActivationOwnedTags.AddTag(TAG_State_Idle);

	AZSPlayerCharacter* Character = Cast<AZSPlayerCharacter>(GetAvatarActorFromActorInfo());

	if (Character && Character->PreviewComponent)
	{
		Character->PreviewComponent->StopPreview();
	}
	
}

void UGA_Right_ATK::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);

	// AZSPlayerCharacter* Character = Cast<AZSPlayerCharacter>(GetAvatarActorFromActorInfo());
	//
	// if (Character && Character->PreviewComponent)
	// {
	// 	Character->PreviewComponent->StopPreview();
	// }
	//
	// FireProjectile();
	//
	// PlayMontage();

	EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
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

	//FVector Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector();
	// FVector TargetPos = SpawnLocation + (Forward * ProjectileRange); // 
	FVector TargetPos = GetTargetLocation();
	
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
	
	FVector LaunchVelocity = CalculateLaunchVelocity(SpawnLocation, TargetPos);
	Projectile->InitProjectile_Velocity(LaunchVelocity);

}

void UGA_Right_ATK::PlayMontage()
{

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

FVector UGA_Right_ATK::CalculateLaunchVelocity(const FVector& Start, const FVector& Target)
{
	FVector LaunchVelocity;

	float GravityZ = GetWorld()->GetGravityZ() * 1.0f; 
		
	bool bSuccess = UGameplayStatics::SuggestProjectileVelocity_CustomArc(
		GetWorld(),
		LaunchVelocity,
		Start,
		Target,
		GravityZ,  
		0.5f   // 0~1 (낮은 포물선 ~ 높은 포물선)
	);

	if (!bSuccess)
	{
		return FVector::ZeroVector;
	}

	return LaunchVelocity;

}

TSubclassOf<AZS_bomb> UGA_Right_ATK::GetProjectileClass() const
{
	return GAProjectile;
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
		EventTagTask->EndTask();
	}
}
