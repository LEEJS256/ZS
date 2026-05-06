// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ZS_ItemWidget.h"

#include "Components/Image.h"
#include "DA/ZS_ItemData.h"

void UZS_ItemWidget::SetItemData(UZS_ItemData* InItemData)
{
	if (!InItemData) return;
	ItemData = InItemData;

	if (ItemIcon && InItemData->ItemIcon)
	{
		ItemIcon->SetBrushFromTexture(InItemData->ItemIcon);
	}
}
