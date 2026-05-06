// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ZS_InventorySlot.generated.h"

/**
 * 
 */
class UZS_ItemData;
class USizeBox;
UCLASS()
class ZS_API UZS_InventorySlot : public UUserWidget
{
	GENERATED_BODY()

public:
	// void SetHighLight(FLinearColor Color);
	void SetHighLight(const FLinearColor& Color);

	void SetIndex(int32 InX, int32 InY);
	void SetItem(TObjectPtr<UZS_ItemData> ArgItemData);

	void SetItemSize(int32 Width, int32 Height, float CellSize);
	void SetBlocked(bool bBlocked);
protected:

	UPROPERTY(meta = (BindWidget))
	class USizeBox* RootSizeBox;
	
	UPROPERTY(meta = (BindWidget))
	class UImage* SlotImage;

	//인덱스들
	UPROPERTY()
	int32 Index_X;

	UPROPERTY()
	int32 Index_Y;
};
