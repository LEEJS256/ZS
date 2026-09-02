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
class UZS_NeonProgressBar;

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
	TObjectPtr<UZS_NeonProgressBar> HPBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UZS_NeonProgressBar> StaminaBar;


	// Delegate 콜백
	void OnHealthChanged(const struct FOnAttributeChangeData& Data);
	void OnStaminaChanged(const struct FOnAttributeChangeData& Data);
	// void OnStaminaChanged(const struct FOnAttributeChangeData& Data);
	// 초기값 세팅용
	void UpdateInitialValues();
};
