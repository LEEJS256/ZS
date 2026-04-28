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
struct FInventoryItem
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<UZS_ItemData> ItemData;

	UPROPERTY()
	int32 Count = 1;

	UPROPERTY()
	FIntPoint Origin; // 기준 좌표 (왼쪽 위)

	UPROPERTY()
	TArray<FIntPoint> OccupiedCells; // ShapeOffsets 적용된 결과
};

USTRUCT(BlueprintType)
struct FGridCell
{
	GENERATED_BODY()

	bool bOccupied = false;
	
	// UPROPERTY()
	// TObjectPtr<class UZS_ItemData> Item = nullptr;
	//
	UPROPERTY()
	int32 ItemIndex = INDEX_NONE; 
};

UCLASS()
class ZS_API UZS_InventoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	//BP에서 생성시 1회만 실행
	void NativeConstruct() override;

	//에디터 프리뷰용임
	void NativePreConstruct() override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
	void RefreshGrid();
	

	virtual void SynchronizeProperties() override;
private:
	void MakeGrid();
protected:
	UPROPERTY()
	TArray<FInventoryItem> Items;
	
	UPROPERTY()
	TArray<FGridCell> ItemGrid;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32  Grid_Num = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32  Grid_Width = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32  Grid_Height = 8;

	// UPROPERTY(meta = (BindWidget))
	// class UGridPanel* GridPanel;

	UPROPERTY(meta = (BindWidget))
	class UUniformGridPanel* GridPanel;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UZS_InventorySlot> SlotClass;
};
