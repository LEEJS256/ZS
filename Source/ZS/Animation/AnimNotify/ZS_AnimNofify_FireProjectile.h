// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimNotify/ZS_AnimNofifyBase.h"
#include "ZS_AnimNofify_FireProjectile.generated.h"

/**
 * 
 */
UCLASS()
class ZS_API UZS_AnimNofify_FireProjectile : public UZS_AnimNofifyBase
{
	GENERATED_BODY()

protected:
	UZS_AnimNofify_FireProjectile();
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

	
};
