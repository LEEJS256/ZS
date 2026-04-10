// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GE_Execute_Damage.generated.h"


UCLASS()
class ZS_API UGE_Execute_Damage : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	UGE_Execute_Damage();

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	                                    FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;

protected:
	//데미지계수
	UPROPERTY(EditDefaultsOnly)
	float DamageCoefficient = 1.0f;
};
