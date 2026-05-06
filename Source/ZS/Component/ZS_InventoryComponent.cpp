// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/ZS_InventoryComponent.h"

#include "DA/ZS_ItemData.h"

// Sets default values for this component's properties
UZS_InventoryComponent::UZS_InventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

bool UZS_InventoryComponent::AddItem(UZS_ItemData* ItemData)
{
	for (int32 y = 0; y < GridHeight; y++)
	{
		for (int32 x = 0; x < GridWidth; x++)
		{
			if (PlaceItem(ItemData, FIntPoint(x, y)))
			{
				OnInventoryChanged.Broadcast();
				return true;
			}
		}
	}

	return false; // 실패
}

bool UZS_InventoryComponent::PlaceItem(UZS_ItemData* ItemData, FIntPoint Origin)
{
	if (!CanPlaceItem(ItemData, Origin))
		return false;

	FInventoryItem_Component NewItem;
	NewItem.ItemData = ItemData;
	NewItem.Origin = Origin;

	int32 NewIndex = Items.Num();

	for (const FIntPoint& Offset : ItemData->ShapeOffsets)
	{
		FIntPoint Cell = Origin + Offset;
		int32 GridIndex = Cell.Y * GridWidth + Cell.X;

		ItemGrid[GridIndex].bOccupied = true;
		ItemGrid[GridIndex].ItemIndex = NewIndex;

		NewItem.OccupiedCells.Add(Cell);
	}

	Items.Add(NewItem);

	return true;
}

bool UZS_InventoryComponent::CanPlaceItem(UZS_ItemData* ItemData, FIntPoint Origin)
{
	for (const FIntPoint& Offset : ItemData->ShapeOffsets)
	{
		FIntPoint Cell = Origin + Offset;

		if (Cell.X < 0 || Cell.X >= GridWidth ||
			Cell.Y < 0 || Cell.Y >= GridHeight)
		{
			return false;
		}

		int32 Index = Cell.Y * GridWidth + Cell.X;

		if (ItemGrid[Index].bOccupied)
			return false;
	}

	return true;
}

void UZS_InventoryComponent::ClearGrid()
{
	for (FGridCell_Component& Cell : ItemGrid)
	{
		Cell.bOccupied = false;
		Cell.ItemIndex = INDEX_NONE;
	}
}


// Called when the game starts
void UZS_InventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	ItemGrid.SetNum(GridWidth * GridHeight);
	// ...
	
}


// Called every frame
void UZS_InventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

