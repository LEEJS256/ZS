// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ZS_InventoryWidget.generated.h"

/**
 * 
 */
class UZS_InventoryWidget;

USTRUCT(BlueprintType)
struct FGridCell
{
	GENERATED_BODY()

	bool bOccupied = false;
	// class UZS_ItemData* Item;

	UPROPERTY()
	TObjectPtr<class UZS_ItemData> Item = nullptr;
};

UCLASS()
class ZS_API UZS_InventoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void NativeConstruct() override;

	void NativePreConstruct() override;

	void RefreshGrid();
	UPROPERTY()
	TArray<FGridCell> ItemGrid;

	virtual void SynchronizeProperties() override;
private:
	void MakeGrid();
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Inventory")
	int32  Grid_Num = 0;
	
	// UPROPERTY(meta = (BindWidget))
	// class UGridPanel* GridPanel;

	UPROPERTY(meta = (BindWidget))
	class UUniformGridPanel* GridPanel;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UZS_InventorySlot> SlotClass;
};
