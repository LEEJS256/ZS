// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ZS_InventoryWidget.generated.h"

/**
 * 
 */
class UZS_InventoryWidget;
class UZS_ItemData;
class UZS_InventoryComponent;
class UCanvasPanel;
class UZS_ItemWidget;
UCLASS()
class ZS_API UZS_InventoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	//BP에서 생성시 1회만 실행
	void NativeConstruct() override;

	//에디터 프리뷰용임
	void NativePreConstruct() override;

	void SetInventory(UZS_InventoryComponent* InInventory);

	void RefreshGrid();

protected:
	UPROPERTY()
	TObjectPtr<UZS_InventoryComponent> InventoryRef;

	UPROPERTY(meta = (BindWidget))
	class UUniformGridPanel* GridPanel;

  	UPROPERTY(meta = (BindWidget))
    class UCanvasPanel* ItemLayer;

	UPROPERTY(EditAnywhere, Category="Inventory")
	TSubclassOf<class UZS_ItemWidget> ItemWidgetClass;  

	UPROPERTY(EditAnywhere, Category="Inventory")
	float CellSize = 128.f;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UZS_InventorySlot> SlotClass;
};
