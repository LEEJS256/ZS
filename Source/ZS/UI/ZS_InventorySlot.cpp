// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ZS_InventorySlot.h"

#include "Components/Image.h"


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
