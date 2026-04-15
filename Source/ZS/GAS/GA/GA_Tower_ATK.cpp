// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA/GA_Tower_ATK.h"

#include "Object/ZSProjectile.h"
#include "Object/ZSProjectile_Chase.h"
#include "Object/ZSTower.h"

UGA_Tower_ATK::UGA_Tower_ATK()
{
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Tower.ATK")));
}

void UGA_Tower_ATK::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
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

	FireProjectile();
}

void UGA_Tower_ATK::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                               const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
                               bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Tower_ATK::FireProjectile()
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!IsValid(AvatarActor))
		return;

	AZSTower* BuildedTower = Cast<AZSTower>(AvatarActor);
	if (!IsValid(BuildedTower))
		return;

	if (!GAProjectile)
		return;

	// 멀티플레이 기준이면 서버에서만 생성
	if (!AvatarActor->HasAuthority())
		return;

	TArray<FVector> SpawnLocation = GetSpawnLocationArray();
	// const FRotator SpawnRotation = GetSpawnRotation();
	AActor* Target = BuildedTower->GetLockOnTarget(); // 네가 만든 타겟

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = BuildedTower;
	// SpawnParams.Instigator = BuildedTower;

	for (const FVector& Socket_Location: SpawnLocation)
	{
		if (!Target) continue;

		FVector Dir = Target->GetActorLocation() - Socket_Location;
		FRotator Rot = Dir.Rotation();
		
		SpawnProjectile(Socket_Location,Rot,SpawnParams,Target);
	}

}

TArray<FVector> UGA_Tower_ATK::GetSpawnLocationArray()
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	AZSTower* BuildedTower = Cast<AZSTower>(AvatarActor);

	TArray<FVector> ReturnLocation;
	// FVector SpawnLocation = FVector::ZeroVector;
	if (!IsValid(BuildedTower))
		return ReturnLocation;

	UStaticMeshComponent* StaticMeshComp = BuildedTower->Barrel;

	if (!StaticMeshComp)
		return ReturnLocation;

	// TArray<FName> SocketName = {"Muzzle_01", "Muzzle_02", "Muzzle_03"};

	for (const FName& Name : MuzzleSockets)
	{
		if (StaticMeshComp->DoesSocketExist(Name))
		{
			ReturnLocation.Add(StaticMeshComp->GetSocketLocation(Name));
		}
	}
	return ReturnLocation;
}


FRotator UGA_Tower_ATK::GetSpawnRotation(int32 LeftRightNum) const
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
}

void UGA_Tower_ATK::SpawnProjectile(FVector Location, FRotator Rotator,FActorSpawnParameters Param,AActor* Target)
{
	AActor* SpawnedProjectile = GetWorld()->SpawnActor<AActor>(
		GAProjectile,
		Location,
		Rotator,
		Param
	);
	
	AZSProjectile_Chase* Projectile = Cast<AZSProjectile_Chase>(SpawnedProjectile);
	if (!Projectile)
		return;
	
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC) return;

	
	FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
	Context.AddSourceObject(this);
	
	Projectile->Set_GE(DamageEffectClass, Context);
	Projectile->SetTarget(Target);
}
