// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ZS_InventorySlot.generated.h"

/**
 * 
 */
UCLASS()
class ZS_API UZS_InventorySlot : public UUserWidget
{
	GENERATED_BODY()

public:
	// void SetHighLight(FLinearColor Color);
	void SetHighLight(const FLinearColor& Color);

	void SetIndex(int32 InX, int32 InY);
protected:

	UPROPERTY(meta = (BindWidget))
	class UImage* SlotImage;
	
	UPROPERTY()
	int32 X;

	UPROPERTY()
	int32 Y;
};
