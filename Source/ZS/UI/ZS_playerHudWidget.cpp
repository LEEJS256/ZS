// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ZS_playerHudWidget.h"
#include "Components/TextBlock.h"
#include "AbilitySystemComponent.h"
#include "Character/ZSPlayerCharacter.h"
#include "Components/ProgressBar.h"
#include "GAS/Attribute/ZSAttributeSet.h"
#include "Slate/ZS_NeonProgressBar.h"


void UZS_playerHudWidget::UpdateHUD(float CurrentHP, float MaxHP, float CurrentStamina, float MaxStamina)
{
	if (HPBar && MaxHP > 0.0f)
	{
		const float HPPercent = FMath::Clamp(CurrentHP / MaxHP, 0.0f, 1.0f);
		HPBar->SetPercent(HPPercent);
	}

	if (StaminaBar && MaxStamina > 0.0f)
	{
		const float StaminaPercent = FMath::Clamp(CurrentStamina / MaxStamina, 0.0f, 1.0f);
		StaminaBar->SetPercent(StaminaPercent);
	}
}

void UZS_playerHudWidget::Init(AActor* OwnerActor)
{
	if (!OwnerActor)
		return;

	// ASC 가져오기
	ASC = Cast<AZSPlayerCharacter>(OwnerActor)->GetAbilitySystemComponent();

	if (!ASC)
		return;

	// AttributeSet 가져오기
	AttributeSet = ASC->GetSet<UZSAttributeSet>();

	if (!AttributeSet)
		return;

	ASC->GetGameplayAttributeValueChangeDelegate(
			UZSAttributeSet::GetHealthAttribute()
		).AddUObject(this, &UZS_playerHudWidget::OnHealthChanged);

	ASC->GetGameplayAttributeValueChangeDelegate(
		UZSAttributeSet::GetStaminaAttribute()
	).AddUObject(this, &UZS_playerHudWidget::OnStaminaChanged);
	//
	// ASC->GetGameplayAttributeValueChangeDelegate(
	// 	UZSAttributeSet::GetStaminaAttribute()
	// ).AddUObject(this, &UZS_playerHudWidget::OnStaminaChanged);

	UpdateInitialValues();
}
//
// void UZS_playerHudWidget::OnHealthChanged(const struct FOnAttributeChangeData& Data)
// {
// 	if (HealthText)
// 	{
// 		// HealthText->SetText(FText::AsNumber(Data.NewValue));
// 		HealthText->SetText(
// 			FText::FromString(FString::Printf(TEXT("Health: %.0f"), Data.NewValue)));
//
// 		float MaxHP = ASC->GetNumericAttribute(UZSAttributeSet::GetMaxHealthAttribute());
//
// 		UpdateHUD(Data.NewValue, MaxHP,
// 			ASC->GetNumericAttribute(UZSAttributeSet::GetStaminaAttribute()),
// 			ASC->GetNumericAttribute(UZSAttributeSet::GetMaxStaminaAttribute()));
// 	}
// }
//
// void UZS_playerHudWidget::OnSpeedChanged(const struct FOnAttributeChangeData& Data)
// {
// 	if (SpeedText)
// 	{
// 		// SpeedText->SetText(FText::AsNumber(Data.NewValue));
// 		SpeedText->SetText(
// 			FText::FromString(FString::Printf(TEXT("Speed: %.0f"), Data.NewValue)));
// 	}
// }
//
// void UZS_playerHudWidget::OnStaminaChanged(const struct FOnAttributeChangeData& Data)
// {
// 	if (StamintText)
// 	{
// 		//StamintText->SetText(FText::AsNumber(Data.NewValue));
// 		StamintText->SetText(
// 			FText::FromString(FString::Printf(TEXT("Stamina: %.0f"), Data.NewValue)));
//
// 		float MaxStamina = ASC->GetNumericAttribute(UZSAttributeSet::GetMaxStaminaAttribute());
//
// 		UpdateHUD(
// 		ASC->GetNumericAttribute(UZSAttributeSet::GetHealthAttribute()),
// 		ASC->GetNumericAttribute(UZSAttributeSet::GetMaxHealthAttribute()),
// 			Data.NewValue,
// 			MaxStamina);
// 	}
// }

void UZS_playerHudWidget::OnHealthChanged(const FOnAttributeChangeData& Data)
{
	if (!ASC)
		return;

	const float MaxHP = ASC->GetNumericAttribute(UZSAttributeSet::GetMaxHealthAttribute());
	const float CurStam = ASC->GetNumericAttribute(UZSAttributeSet::GetStaminaAttribute());
	const float MaxStam = ASC->GetNumericAttribute(UZSAttributeSet::GetMaxStaminaAttribute());

	UpdateHUD(Data.NewValue, MaxHP, CurStam, MaxStam);
}

void UZS_playerHudWidget::OnStaminaChanged(const FOnAttributeChangeData& Data)
{
	if (!ASC)
		return;

	const float CurHP = ASC->GetNumericAttribute(UZSAttributeSet::GetHealthAttribute());
	const float MaxHP = ASC->GetNumericAttribute(UZSAttributeSet::GetMaxHealthAttribute());
	const float MaxStam = ASC->GetNumericAttribute(UZSAttributeSet::GetMaxStaminaAttribute());

	UpdateHUD(CurHP, MaxHP, Data.NewValue, MaxStam);
}

void UZS_playerHudWidget::UpdateInitialValues()
{
	if (!ASC)
		return;

	const float CurHP = ASC->GetNumericAttribute(UZSAttributeSet::GetHealthAttribute());
	const float MaxHP = ASC->GetNumericAttribute(UZSAttributeSet::GetMaxHealthAttribute());
	const float CurStam = ASC->GetNumericAttribute(UZSAttributeSet::GetStaminaAttribute());
	const float MaxStam = ASC->GetNumericAttribute(UZSAttributeSet::GetMaxStaminaAttribute());

	UpdateHUD(CurHP, MaxHP, CurStam, MaxStam);
}
