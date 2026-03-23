// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ZS_playerHudWidget.h"
#include "Components/TextBlock.h"
#include "AbilitySystemComponent.h"
#include "Character/ZSPlayerCharacter.h"
#include "Components/ProgressBar.h"
#include "GAS/Attribute/ZSAttributeSet.h"


void UZS_playerHudWidget::UpdateHUD(float CurrentHP, float MaxHP, float CurrentStamina, float MaxStamina)
{
	if (HPBar && MaxHP > 0)
	{
		HPBar->SetPercent(CurrentHP / MaxHP);
	}

	if (StaminaBar && MaxStamina > 0)
	{
		StaminaBar->SetPercent(CurrentStamina / MaxStamina);
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
		UZSAttributeSet::GetSpeedAttribute()
	).AddUObject(this, &UZS_playerHudWidget::OnSpeedChanged);

	ASC->GetGameplayAttributeValueChangeDelegate(
		UZSAttributeSet::GetStaminaAttribute()
	).AddUObject(this, &UZS_playerHudWidget::OnStaminaChanged);

	UpdateInitialValues();
}

void UZS_playerHudWidget::OnHealthChanged(const struct FOnAttributeChangeData& Data)
{
	if (HealthText)
	{
		// HealthText->SetText(FText::AsNumber(Data.NewValue));
		HealthText->SetText(
			FText::FromString(FString::Printf(TEXT("Health: %.0f"), Data.NewValue)));
	}
}

void UZS_playerHudWidget::OnSpeedChanged(const struct FOnAttributeChangeData& Data)
{
	if (SpeedText)
	{
		// SpeedText->SetText(FText::AsNumber(Data.NewValue));
		SpeedText->SetText(
			FText::FromString(FString::Printf(TEXT("Speed: %.0f"), Data.NewValue)));
	}
}

void UZS_playerHudWidget::OnStaminaChanged(const struct FOnAttributeChangeData& Data)
{
	if (StamintText)
	{
		//StamintText->SetText(FText::AsNumber(Data.NewValue));
		StamintText->SetText(
			FText::FromString(FString::Printf(TEXT("Stamina: %.0f"), Data.NewValue)));
	}
}

void UZS_playerHudWidget::UpdateInitialValues()
{
	if (!ASC)
		return;

	float Health = ASC->GetNumericAttribute(UZSAttributeSet::GetHealthAttribute());
	float Speed = ASC->GetNumericAttribute(UZSAttributeSet::GetSpeedAttribute());
	float Stamina	= ASC->GetNumericAttribute(UZSAttributeSet::GetStaminaAttribute());
;	if (HealthText)
		HealthText->SetText(FText::AsNumber(Health));

	if (SpeedText)
		SpeedText->SetText(FText::AsNumber(Speed));

	if (StamintText)
		StamintText->SetText(FText::AsNumber(Stamina));
}
