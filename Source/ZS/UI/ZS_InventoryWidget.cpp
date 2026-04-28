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
	
	RefreshGrid();
}

void UZS_InventoryWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	RefreshGrid();
}

#if WITH_EDITOR
void UZS_InventoryWidget::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	static const FName WidthName  = GET_MEMBER_NAME_CHECKED(UZS_InventoryWidget, Grid_Width);
	static const FName HeightName = GET_MEMBER_NAME_CHECKED(UZS_InventoryWidget, Grid_Height);

	FName ChangedName = PropertyChangedEvent.GetPropertyName();
    
	if (ChangedName == WidthName || ChangedName == HeightName)
	{
		if (GridPanel && SlotClass)
		{
			GridPanel->ClearChildren();
			ItemGrid.SetNum(Grid_Width * Grid_Height);
			MakeGrid();
		}
	}
}
#endif

void UZS_InventoryWidget::RefreshGrid()
{
	if (!GridPanel || !SlotClass)
		return;
	GridPanel->ClearChildren();
	ItemGrid.SetNum(Grid_Width * Grid_Height);
	MakeGrid();
}

void UZS_InventoryWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	// UE_LOG(LogTemp, Warning, TEXT("SynchronizeProperties called: W=%d H=%d"), Grid_Width, Grid_Height);
	
	if (!GridPanel || !SlotClass)
		return;
	
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
	// UE_LOG(LogTemp,Warning, TEXT("Make Grid: W=%d H=%d"), Grid_Width, Grid_Height);

	for (int32 Y = 0; Y < Grid_Height; Y++)
	{
		for (int32 X = 0; X < Grid_Width; X++)
		{
			UZS_InventorySlot* newInventorySlot = CreateWidget<UZS_InventorySlot>(GetWorld(), SlotClass);

			if (!newInventorySlot)
				return;

			newInventorySlot->SetIndex(X, Y);
			
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
