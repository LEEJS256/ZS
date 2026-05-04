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
	X = InX;
	Y = InY;
}

void UZS_InventorySlot::SetItem(TObjectPtr<UZS_ItemData> ArgItemData)
{
	if (!SlotImage)
		return;

	if (SlotImage && ArgItemData->ITemIcon)
	{
		SlotImage->SetBrushFromTexture(ArgItemData->ITemIcon);
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
}

void UZS_InventorySlot::SetBlocked(bool bBlocked)
{
	if (bBlocked)
	{
		SetVisibility(ESlateVisibility::Hidden); // 또는 HitTestInvisible
	}
	else
	{
		SetVisibility(ESlateVisibility::Visible);
	}
}
