// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ZS_InventoryWidget.h"

#include "ZS_InventorySlot.h"
#include "Component/ZS_InventoryComponent.h"
#include "Components/Image.h"
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
}


void UZS_InventoryWidget::RefreshGrid()
{
	if (!GridPanel || !SlotClass || !InventoryRef)
		return;

	GridPanel->ClearChildren();

	const auto& Grid = InventoryRef->GetGrid();
	const auto& Items = InventoryRef->GetItems();

	int32 GridWidth = InventoryRef->GridWidth;

	for (int32 i = 0; i < Grid.Num(); ++i)
	{
		int32 X = i % GridWidth;
		int32 Y = i / GridWidth;

		UZS_InventorySlot* NewSlot = CreateWidget<UZS_InventorySlot>(this, SlotClass);

		bool bBlocked = false;

		if (Grid[i].ItemIndex != INDEX_NONE)
		{
			const auto& Item = Items[Grid[i].ItemIndex];

			if (Item.Origin.X == X && Item.Origin.Y == Y)
			{
				// ⭐ 대표 칸
				NewSlot->SetItem(Item.ItemData);

				int32 Width = 0;
				int32 Height = 0;

				for (const FIntPoint& Offset : Item.ItemData->ShapeOffsets)
				{
					Width = FMath::Max(Width, Offset.X + 1);
					Height = FMath::Max(Height, Offset.Y + 1);
				}

				NewSlot->SetItemSize(Width, Height, 64.f);
			}
			else
			{
				// ⭐ 나머지 칸 막기 (핵심!)
				bBlocked = true;
			}
		}

		NewSlot->SetBlocked(bBlocked);

		GridPanel->AddChildToUniformGrid(NewSlot, Y, X);
	}
}