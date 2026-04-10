// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "WBP_Damage_Text.generated.h"

class UWidgetAnimation;
class UTextBlock;

/**
 * 
 */
UCLASS()
class ZS_API UWBP_Damage_Text : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitDamage(float Damage,bool bCritical,FGameplayTag DamageType);
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* DamageText;

	void SetDamamge(float Damage);

	FTimerHandle DeleteTimer;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* FloatUpAnim;
};
