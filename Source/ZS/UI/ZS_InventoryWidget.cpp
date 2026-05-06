// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ZS_InventoryWidget.h"

#include "ZS_InventorySlot.h"
#include "ZS_ItemWidget.h"
#include "Component/ZS_InventoryComponent.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/UniformGridPanel.h"
#include "DA/ZS_ItemData.h"


void UZS_InventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();


	if (!GridPanel || !SlotClass)
		return;


	RefreshGrid();
}

void UZS_InventoryWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	RefreshGrid();
}

void UZS_InventoryWidget::SetInventory(UZS_InventoryComponent* InInventory)
{
	InventoryRef = InInventory;
	InventoryRef->OnInventoryChanged.AddDynamic(this, &UZS_InventoryWidget::RefreshGrid);
}


void UZS_InventoryWidget::RefreshGrid()
{
	if (!GridPanel || !SlotClass || !InventoryRef)
		return;

	GridPanel->ClearChildren();

	if (ItemLayer)
		ItemLayer->ClearChildren();

	
	int32 Cols = InventoryRef->GridWidth;
	int32 Rows = InventoryRef->GridHeight;

	float TotalWidth  = Cols * CellSize;
	float TotalHeight = Rows * CellSize;

	if (UCanvasPanelSlot* GridSlot = Cast<UCanvasPanelSlot>(GridPanel->Slot))
	{
		// GridSlot->SetAutoSize(false);
		GridSlot->SetPosition(FVector2D(0.f, 0.f));
		GridSlot->SetSize(FVector2D(TotalWidth, TotalHeight));
	}
	if (UCanvasPanelSlot* LayerSlot = Cast<UCanvasPanelSlot>(ItemLayer->Slot))
	{
		// LayerSlot->SetAutoSize(false);
		LayerSlot->SetSize(FVector2D(TotalWidth, TotalHeight));
		LayerSlot->SetPosition(FVector2D(0.f, 0.f));
	}
	
	// 1단계: 배경 슬롯
	for (int32 Y = 0; Y < Rows; Y++)
	{
		for (int32 X = 0; X < Cols; X++)
		{
			UZS_InventorySlot* NewSlot = CreateWidget<UZS_InventorySlot>(this, SlotClass);
			if (!NewSlot) continue;

			NewSlot->SetIndex(X, Y);
			NewSlot->SetItemSize(1, 1, CellSize);
			NewSlot->SetBlocked(false);

			GridPanel->AddChildToUniformGrid(NewSlot, Y, X);
		}
	}

	// 2단계: 아이템 배치
	if (!ItemLayer || !ItemWidgetClass) return;

	for (const auto& Item : InventoryRef->GetItems())
	{
		if (!Item.ItemData) continue;

		UZS_ItemWidget* ItemWidget = CreateWidget<UZS_ItemWidget>(this, ItemWidgetClass);
		ItemWidget->SetItemData(Item.ItemData);

		UCanvasPanelSlot* CanvasSlot = ItemLayer->AddChildToCanvas(ItemWidget);

		int32 W = Item.ItemData->GetPlacedWidth();
		int32 H = Item.ItemData->GetPlacedHeight();

		float PosX = Item.Origin.X * CellSize;
		float PosY = Item.Origin.Y * CellSize;

		if (Item.ItemData->bRotated)
		{
			ItemWidget->SetRenderTransformAngle(90.f);
			ItemWidget->SetRenderTransformPivot(FVector2D(0.f, 0.f));
			PosX += H * CellSize;
		}

		CanvasSlot->SetPosition(FVector2D(PosX, PosY));
		CanvasSlot->SetSize(FVector2D(W * CellSize, H * CellSize));
		CanvasSlot->SetAutoSize(false);
	}
}
