// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ZS_ItemWidget.h"

#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "DA/ZS_ItemData.h"
#include "UI_Utility/ZS_ItemDragDrop.h"

void UZS_ItemWidget::SetItemData(UZS_ItemData* InItemData)
{
	if (!InItemData) return;
	ItemData = InItemData;

	if (ItemIcon && InItemData->ItemIcon)
	{
		ItemIcon->SetBrushFromTexture(InItemData->ItemIcon);
	}
}

void UZS_ItemWidget::SetOrigin(FIntPoint InOrigin)
{
	Origin = InOrigin;
}

void UZS_ItemWidget::SetCellSize(float InCellSize)
{
	CellSize = InCellSize;
}

void UZS_ItemWidget::SetCount(int32 InCount)
{
	CachedCount = InCount;
	
	if (!CountText)
		return;
 
	if (InCount > 1)
	{
		CountText->SetText(FText::AsNumber(InCount));
		CountText->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	else
	{
		CountText->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UZS_ItemWidget::NativeConstruct()
{
	Super::NativeConstruct();
	SetCount(CachedCount);
}

FReply UZS_ItemWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	// 좌클릭만 드래그 시작
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		return FReply::Handled().DetectDrag(TakeWidget(), EKeys::LeftMouseButton);
	}
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UZS_ItemWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent,
                                          UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	if (!ItemData) return;

	UZS_ItemDragDrop* DragOp = NewObject<UZS_ItemDragDrop>(this);
	DragOp->ItemData       = ItemData;
	DragOp->OriginalOrigin = Origin;

	FVector2D LocalMousePos = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
	DragOp->DragOffset     = LocalMousePos;

	UZS_ItemWidget* DragVisual = CreateWidget<UZS_ItemWidget>(GetOwningPlayer(), GetClass());
	DragVisual->SetItemData(ItemData);
	DragVisual->SetCellSize(CellSize);

	int32 W = ItemData->GetPlacedWidth();
	int32 H = ItemData->GetPlacedHeight();
	
	if (USizeBox* SizeBox = Cast<USizeBox>(DragVisual->GetWidgetFromName(TEXT("RootSizeBox"))))
	{
		SizeBox->SetWidthOverride(W * CellSize);
		SizeBox->SetHeightOverride(H * CellSize);
	}

	if (ItemData->bRotated)
	{
		DragVisual->SetRenderTransformAngle(90.f);
		DragVisual->SetRenderTransformPivot(FVector2D(0.f, 0.f));
	}

	DragOp->DefaultDragVisual = DragVisual;
	DragOp->Pivot             = EDragPivot::MouseDown;

	OutOperation = DragOp;

	//안보이게하기
	//SetVisibility(ESlateVisibility::Hidden);
	//알파값 0.5
	SetRenderOpacity(0.5f);
}
