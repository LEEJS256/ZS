// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ZS_StatusWidget.h"

#include "Components/TextBlock.h"

void UZS_StatusWidget::UpdateStatus(const FZS_StatData& Data)
{
	Name_Text->SetText(
		FText::Format(
			FText::FromString("Name: {0}"),
			FText::FromName(Data.PlayerName)
		)
	);
	
	Level_Text->SetText(FText::FromString(
	  FString::Printf(TEXT("Level: %d"), Data.Level)));

	Health_Text->SetText(FText::FromString(
		FString::Printf(TEXT("Health: %.0f / %.0f"), Data.HP, Data.MaxHP)));

	Stamina_Text->SetText(FText::FromString(
		FString::Printf(TEXT("Stamina: %.0f / %.0f"), Data.Stamina, Data.MaxStamina)));
	
	Gold_Text->SetText(FText::FromString(
		FString::Printf(TEXT("Gold: %d"), Data.Gold)));

	Exp_Text->SetText(FText::FromString(
		FString::Printf(TEXT("Exp: %d / %d"), Data.Exp, Data.MaxExp)));

	Attack_Text->SetText(FText::FromString(
		FString::Printf(TEXT("ATK: %.0f"), Data.Attack)));

	Armor_Text->SetText(FText::FromString(
		FString::Printf(TEXT("Armor: %.0f"), Data.Armor)));
}
