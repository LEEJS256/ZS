// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ZS_ItemWidget.generated.h"

/**
 * 
 */
class UZS_ItemData;
class UImage;
class USizeBox;

UCLASS()
class ZS_API UZS_ItemWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetItemData(UZS_ItemData* InItemData);

protected:
	// BP에서 SizeBox > Image 구조로 만들면 됨
	UPROPERTY(meta = (BindWidget))
	USizeBox* RootSizeBox;

	UPROPERTY(meta = (BindWidget))
	UImage* ItemIcon;

	UPROPERTY()
	TObjectPtr<UZS_ItemData> ItemData;
};
	

