// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/GA/ZSGameplayAbility.h"
#include "Object/ZS_bomb.h"
#include "GA_Right_ATK.generated.h"

/**
 * 
 */
class UAbilityTask_WaitGameplayEvent;
UCLASS()
class ZS_API UGA_Right_ATK : public UZSGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_Right_ATK();

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled) override;

	virtual void InputReleased(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo) override;
	
	void FireProjectile();
	void PlayMontage();
	FVector GetSpawnLocation() const;
	FRotator GetSpawnRotationFromCrossHair();
	FVector GetTargetLocation() const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Projectile")
	float ProjectileSpeed = 1500.f;

	FVector CalculateLaunchVelocity(const FVector& Start, const FVector& Target);

	TSubclassOf<AZS_bomb> GetProjectileClass() const;
protected:

	UPROPERTY(EditAnywhere)
	TArray<FName> RightHand_Socket;
	
	UPROPERTY()
	UAbilityTask_WaitGameplayEvent* EventTagTask;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Weapon|Animation|Attack")
	TArray<TObjectPtr<UAnimMontage>> AttackMontages;
		
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Projectile")
	TSubclassOf<AZS_bomb> GAProjectile;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Projectile")
	float ProjectileRange = 2000.f;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Damage")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Projectile")
	float ThrowAngle = 0.5f;

	
	UFUNCTION()
	void OnMontageCompleted();

	UFUNCTION()
	void OnMontageInterrupted();

	UFUNCTION()
	void OnFireEvent(FGameplayEventData Payload);

	bool bIsAiming = false;
	
};


