// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WBP_Damage_Text.h"

#include "Components/TextBlock.h"
#include "Utility/ZSNativeGameplayTag.h"

void UWBP_Damage_Text::InitDamage(float Damage, bool bCritical, FGameplayTag DamageType)
{
	DamageText->SetText(FText::AsNumber(static_cast<int32>(Damage)));

	// 🔹 색상
	FLinearColor Color = FLinearColor::White;

	if (DamageType == TAG_DMGType_Fire)
		Color = FLinearColor::Red;
	else if (DamageType == TAG_DMGType_Ice)
		Color = FLinearColor::Blue;

	DamageText->SetColorAndOpacity(FSlateColor(Color));

	// 🔹 크리티컬
	if (bCritical)
	{
		DamageText->SetRenderScale(FVector2D(1.5f, 1.5f));
		// DamageText->SetFont(FSlateFontInfo("Roboto", 36)); // 크게
		// PlayAnimation(CriticalAnim); // (선택)
	}
	// else
	// {
	// 	DamageText->SetFont(FSlateFontInfo("Roboto", 24));
	// }

	// 🔹 기본 애니메이션
	PlayAnimation(FloatUpAnim);


	GetWorld()->GetTimerManager().SetTimer(
		DeleteTimer,
		this,
		&UWBP_Damage_Text::RemoveFromParent,
		1.0f,
		false
	);
	
}

void UWBP_Damage_Text::SetDamamge(float Damage)
{
	if (DamageText)
	{
		DamageText->SetText(FText::AsNumber((int32)Damage));
	}
}
