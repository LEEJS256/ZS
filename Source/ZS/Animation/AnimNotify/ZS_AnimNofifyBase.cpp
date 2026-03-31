// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotify/ZS_AnimNofifyBase.h"

UZS_AnimNofifyBase::UZS_AnimNofifyBase()
{
}

void UZS_AnimNofifyBase::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
}
