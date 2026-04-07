// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ZS_MonsterHealthWidget.generated.h"

/**
 * 
 */
UCLASS()
class ZS_API UZS_MonsterHealthWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
	void SetHealthPercent(float Percent);
};
