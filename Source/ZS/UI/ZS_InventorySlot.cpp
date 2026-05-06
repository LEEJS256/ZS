// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ZS_InventorySlot.h"
#include "DA/ZS_ItemData.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"


void UZS_InventorySlot::SetHighLight(const FLinearColor& Color)
{
	if (!SlotImage)
		return;

	SlotImage->SetColorAndOpacity(Color);
}

void UZS_InventorySlot::SetIndex(int32 InX, int32 InY)
{
	Index_X = InX;
	Index_Y = InY;
}

void UZS_InventorySlot::SetItem(TObjectPtr<UZS_ItemData> ArgItemData)
{
	if (!SlotImage)
		return;

	if (SlotImage && ArgItemData->ItemIcon)
	{
		SlotImage->SetBrushFromTexture(ArgItemData->ItemIcon);
		SlotImage->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		SlotImage->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UZS_InventorySlot::SetItemSize(int32 Width, int32 Height, float CellSize)
{
	if (!RootSizeBox)
		return;

	RootSizeBox->SetWidthOverride(Width * CellSize);
	RootSizeBox->SetHeightOverride(Height * CellSize);

	// if (SlotImage)
	// {
	// 	// 브러시가 없어도 단색으로 렌더링되도록 강제 설정
	// 	FSlateBrush Brush;
	// 	Brush.DrawAs = ESlateBrushDrawType::RoundedBox;
	// 	Brush.TintColor = FSlateColor(FLinearColor(0.1f, 0.1f, 0.1f, 0.8f));
	// 	Brush.OutlineSettings.Width = 1.f;
	// 	Brush.OutlineSettings.Color = FSlateColor(FLinearColor(1.f, 1.f, 1.f, 0.5f));
	// 	SlotImage->SetBrush(Brush);
	// }
	
	// if (SlotImage)
	// {
	// 	// 슬롯 배경색 설정 (회색 반투명)
	// 	SlotImage->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 0.2f));
	// }
}

void UZS_InventorySlot::SetBlocked(bool bBlocked)
{
	// if (bBlocked)
	// {
	// 	SetVisibility(ESlateVisibility::Hidden); // 또는 HitTestInvisible
	// }
	// else
	// {
	// 	SetVisibility(ESlateVisibility::Visible);
	// }
}
