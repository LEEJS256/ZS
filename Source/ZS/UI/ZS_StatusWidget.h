// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ZS_StatusWidget.generated.h"

/**
 * 
 */
UCLASS()
class ZS_API UZS_StatusWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	// UZS_StatusWidget();
protected:

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Health_Text;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthBar;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Stamina_Text;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* StaminaBar;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Gold_Text;

	// 경험치 표시 (예: "150 / 1000")
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Exp_Text;

	// 레벨 표시 (예: "Lv. 10")
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Level_Text;

	// 공격력 표시
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Attack_Text;

	// 방어력 표시
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Armor_Text;
};
