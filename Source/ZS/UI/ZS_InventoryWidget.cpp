// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ZS_InventoryWidget.h"

#include "ZS_InventorySlot.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"

void UZS_InventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ItemGrid.SetNum(Grid_Num * Grid_Num);

	if (!GridPanel || !SlotClass)
		return;

}

void UZS_InventoryWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	RefreshGrid();
}

void UZS_InventoryWidget::RefreshGrid()
{
	GridPanel->ClearChildren(); 
	MakeGrid();
}

void UZS_InventoryWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	if (!GridPanel || !SlotClass) return;
	
	if (Grid_Num <= 0) 
	{
		GridPanel->ClearChildren();
		return;
	}
	
	GridPanel->ClearChildren(); 
	MakeGrid();
}

void UZS_InventoryWidget::MakeGrid()
{
	for (int32 Y = 0; Y < Grid_Num; Y++)
	{
		for (int32 X = 0; X < Grid_Num; X++)
		{
			UZS_InventorySlot* newInventorySlot = CreateWidget<UZS_InventorySlot>(GetWorld(), SlotClass);

			if (!newInventorySlot)
				return;

			newInventorySlot->SetIndex(X, Y);

			//UGridSlot* GridSlot = GridPanel->AddChildToGrid(newInventorySlot,Y,X);

			UUniformGridSlot* GridSlot = GridPanel->AddChildToUniformGrid(newInventorySlot, Y, X);


			if (GridSlot)
			{
				// GridSlot->SetPadding((FMargin(0.f)));

				GridSlot->SetHorizontalAlignment(HAlign_Fill);
				GridSlot->SetVerticalAlignment(VAlign_Fill);
			}
		}
	}
}
