// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ZS_playerHudWidget.generated.h"

/**
 *
 * 
 */
class UAbilitySystemComponent;
class UZSAttributeSet;
class UTextBlock;
UCLASS()
class ZS_API UZS_playerHudWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// UI 업데이트 함수
	UFUNCTION(BlueprintCallable)
	void UpdateHUD(float CurrentHP, float MaxHP, float CurrentStamina, float MaxStamina);

	void Init(AActor* OwnerActor);

	
protected:
	
	UPROPERTY()
	UAbilitySystemComponent* ASC;

	UPROPERTY()
	const UZSAttributeSet* AttributeSet;
	
	// ProgressBar 연결
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HPBar;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* StaminaBar;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* HealthText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* StamintText;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* SpeedText;

	
	// Delegate 콜백
	void OnHealthChanged(const struct FOnAttributeChangeData& Data);
	void OnSpeedChanged(const struct FOnAttributeChangeData& Data);
	void OnStaminaChanged(const struct FOnAttributeChangeData& Data);
	// 초기값 세팅용
	void UpdateInitialValues();
};
