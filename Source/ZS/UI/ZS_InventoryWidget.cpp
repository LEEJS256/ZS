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
#include "UI_Utility/ZS_ItemDragDrop.h"


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
		GridSlot->SetPosition(FVector2D(0.f, 0.f));
		GridSlot->SetSize(FVector2D(TotalWidth, TotalHeight));
	}
	if (UCanvasPanelSlot* LayerSlot = Cast<UCanvasPanelSlot>(ItemLayer->Slot))
	{
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
	if (!ItemLayer || !ItemWidgetClass)
		return;
 
	for (const auto& Item : InventoryRef->GetItems())
	{
		if (!Item.ItemData) continue;
 
		UZS_ItemWidget* ItemWidget = CreateWidget<UZS_ItemWidget>(this, ItemWidgetClass);
		ItemWidget->SetItemData(Item.ItemData);
		ItemWidget->SetOrigin(Item.Origin);
		ItemWidget->SetCellSize(CellSize);
		ItemWidget->SetCount(Item.Count);

		if (ActiveDragOp && ActiveDragOp->OriginalOrigin == Item.Origin)
		{
			ItemWidget->SetVisibility(ESlateVisibility::Hidden);
		}
		
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
 
	HighlightedCells.Empty();
	// ActiveDragOp = nullptr;

}

bool UZS_InventoryWidget::NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
                                           UDragDropOperation* InOperation)
{
	UZS_ItemDragDrop* DragOp = Cast<UZS_ItemDragDrop>(InOperation);
	if (!DragOp || !DragOp->ItemData) return false;
 
	ActiveDragOp = DragOp;
 
	FIntPoint TargetCell = GetCellFromAbsolutePosition(
		InGeometry,
		InDragDropEvent.GetScreenSpacePosition(),
		DragOp->DragOffset);
 
	ResetHighlights();
 
	bool bCanPlace = InventoryRef->CanPlaceItemExcluding(
		DragOp->ItemData, TargetCell, DragOp->OriginalOrigin);
 
	HighlightSlots(TargetCell, DragOp->ItemData, bCanPlace);
 
	return true;
}

bool UZS_InventoryWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	UZS_ItemDragDrop* DragOp = Cast<UZS_ItemDragDrop>(InOperation);
	if (!DragOp || !DragOp->ItemData || !InventoryRef) return false;
 
	FIntPoint TargetCell = GetCellFromAbsolutePosition(
		InGeometry,
		InDragDropEvent.GetScreenSpacePosition(),
		DragOp->DragOffset);
 
	bool bSuccess = InventoryRef->MoveItem(DragOp->OriginalOrigin, TargetCell);
 
	ResetHighlights();
	ActiveDragOp = nullptr;
 
	if (!bSuccess)
	{
		RefreshGrid();
	}
 
	return bSuccess;
}

void UZS_InventoryWidget::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	ResetHighlights();
	ActiveDragOp = nullptr;
	}

void UZS_InventoryWidget::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	ResetHighlights();
	ActiveDragOp = nullptr;
	RefreshGrid();
}

// FReply UZS_InventoryWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
// {
// 	if (InKeyEvent.GetKey() == EKeys::R && ActiveDragOp && ActiveDragOp->ItemData)
// 	{
// 		// bRotated 토글
// 		ActiveDragOp->RotationStep = (ActiveDragOp->RotationStep + 1) % 4;
// 		ActiveDragOp->ItemData->RotationStep = ActiveDragOp->RotationStep;
//
//
// 		if (UZS_ItemWidget* DragVisual = Cast<UZS_ItemWidget>(ActiveDragOp->DefaultDragVisual))
// 		{
// 			// float Angle = ActiveDragOp->ItemData->GetRotationAngle();
// 			// DragVisual->SetRenderTransformAngle(Angle);
// 			// DragVisual->SetRenderTransformPivot(FVector2D(0.f, 0.f));
//
// 			int32 Step = ActiveDragOp->RotationStep;
// 			float Angle = ActiveDragOp->ItemData->GetRotationAngle();
// 			int32 W = ActiveDragOp->ItemData->GetPlacedWidth();
// 			int32 H = ActiveDragOp->ItemData->GetPlacedHeight();
//
// 			DragVisual->SetRenderTransformAngle(Angle);
//
// 			// 회전 단계별 pivot 보정
// 			FVector2D Pivot = FVector2D(0.f, 0.f);
// 			switch (Step)
// 			{
// 			case 0: Pivot = FVector2D(0.f, 0.f); break;
// 			case 1: Pivot = FVector2D(0.f, 0.f); break; // X 보정 필요
// 			case 2: Pivot = FVector2D(1.f, 1.f); break;
// 			case 3: Pivot = FVector2D(0.f, 0.f); break; // Y 보정 필요
// 			}
// 			DragVisual->SetRenderTransformPivot(Pivot);
//
// 			// SizeBox 크기도 회전에 맞게 갱신
// 			if (USizeBox* SizeBox = Cast<USizeBox>(DragVisual->GetRootWidget()))
// 			{
// 				SizeBox->SetWidthOverride(W * CellSize);
// 				SizeBox->SetHeightOverride(H * CellSize);
// 			}
// 		}
// 		
// 		// 현재 마우스 위치 기준으로 하이라이트 즉시 갱신
// 		// (다음 OnDragOver에서 자동 반영되므로 별도 처리 불필요)
// 		ResetHighlights();
//
// 		return FReply::Handled();
// 	}
//
// 	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
// }

FIntPoint UZS_InventoryWidget::GetCellFromAbsolutePosition(const FGeometry& InGeometry, FVector2D AbsMousePos,
	FVector2D DragOffset) const
{
	// 절대 좌표 → 위젯 로컬 좌표
	FVector2D LocalPos = InGeometry.AbsoluteToLocal(AbsMousePos);

	// 드래그 오프셋 제거 (아이템 좌상단 기준 정렬)
	FVector2D AdjustedPos = LocalPos - DragOffset;

	int32 CellX = FMath::FloorToInt(AdjustedPos.X / CellSize);
	int32 CellY = FMath::FloorToInt(AdjustedPos.Y / CellSize);

	return FIntPoint(CellX, CellY);
}

void UZS_InventoryWidget::HighlightSlots(FIntPoint Origin, UZS_ItemData* ItemData, bool bCanPlace)
{
	if (!GridPanel || !ItemData)
		return;

	FLinearColor Color = bCanPlace
		? FLinearColor(0.f, 1.f, 0.f, 0.5f)   // 초록 = 배치 가능
		: FLinearColor(1.f, 0.f, 0.f, 0.5f);  // 빨강 = 불가

	TArray<FIntPoint> Offsets = ItemData->GetPlacedOffsets();
	int32 Cols = InventoryRef->GridWidth;
	int32 Rows = InventoryRef->GridHeight;

	for (const FIntPoint& Offset : Offsets)
	{
		FIntPoint Cell = Origin + Offset;

		if (Cell.X < 0 || Cell.X >= Cols || Cell.Y < 0 || Cell.Y >= Rows)
			continue;

		int32 SlotIndex = Cell.Y * Cols + Cell.X;
		if (!GridPanel->GetChildAt(SlotIndex)) continue;

		if (UZS_InventorySlot* NewSlot = Cast<UZS_InventorySlot>(GridPanel->GetChildAt(SlotIndex)))
		{
			NewSlot->SetHighLight(Color);
			HighlightedCells.Add(Cell);
		}
	}
}

void UZS_InventoryWidget::ResetHighlights()
{
	if (!GridPanel || !InventoryRef) return;

	int32 Cols = InventoryRef->GridWidth;

	for (const FIntPoint& Cell : HighlightedCells)
	{
		int32 SlotIndex = Cell.Y * Cols + Cell.X;
		if (UZS_InventorySlot* NewSlot = Cast<UZS_InventorySlot>(GridPanel->GetChildAt(SlotIndex)))
		{
			NewSlot->SetHighLight(FLinearColor::White);
		}
	}

	HighlightedCells.Empty();
}
