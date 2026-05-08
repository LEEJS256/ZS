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

	for (const FIntPoint& Offset : ItemData->GetPlacedOffsets())
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

bool UZS_InventoryComponent::CanPlaceItemExcluding(UZS_ItemData* ItemData, FIntPoint NewOrigin, FIntPoint ExcludeOrigin)
{
	// ExcludeOrigin 에 있는 아이템이 점유한 셀 수집
	TSet<FIntPoint> ExcludedCells;
	int32 ExcludeIdx = FindItemIndexByOrigin(ExcludeOrigin);
	if (ExcludeIdx != INDEX_NONE)
	{
		for (const FIntPoint& Cell : Items[ExcludeIdx].OccupiedCells)
			ExcludedCells.Add(Cell);
	}

	for (const FIntPoint& Offset : ItemData->GetPlacedOffsets())
	{
		FIntPoint Cell = NewOrigin + Offset;

		if (Cell.X < 0 || Cell.X >= GridWidth ||
			Cell.Y < 0 || Cell.Y >= GridHeight)
			return false;

		int32 Index = Cell.Y * GridWidth + Cell.X;

		// 자기 자신의 기존 셀은 점유 무시
		if (ItemGrid[Index].bOccupied && !ExcludedCells.Contains(Cell))
			return false;
	}
	return true;
}

bool UZS_InventoryComponent::MoveItem(FIntPoint OldOrigin, FIntPoint NewOrigin)
{
	int32 ItemIdx = FindItemIndexByOrigin(OldOrigin);
	if (ItemIdx == INDEX_NONE) return false;

	FInventoryItem_Component ItemCopy = Items[ItemIdx];

	// 1) 기존 셀 해제
	for (const FIntPoint& Cell : ItemCopy.OccupiedCells)
	{
		int32 GridIdx = Cell.Y * GridWidth + Cell.X;
		ItemGrid[GridIdx].bOccupied = false;
		ItemGrid[GridIdx].ItemIndex = INDEX_NONE;
	}
	Items.RemoveAt(ItemIdx);

	// ItemIndex 참조 재정렬 (RemoveAt 이후 인덱스 밀림 보정)
	for (int32 i = 0; i < Items.Num(); i++)
	{
		for (const FIntPoint& Cell : Items[i].OccupiedCells)
		{
			int32 GridIdx = Cell.Y * GridWidth + Cell.X;
			ItemGrid[GridIdx].ItemIndex = i;
		}
	}

	// 2) 새 위치에 배치
	if (!PlaceItem(ItemCopy.ItemData, NewOrigin))
	{
		// 실패 시 원래 위치로 복원
		PlaceItem(ItemCopy.ItemData, OldOrigin);
		return false;
	}

	OnInventoryChanged.Broadcast();
	return true;
}

void UZS_InventoryComponent::RemoveItemAt(FIntPoint Origin)
{
	int32 ItemIdx = FindItemIndexByOrigin(Origin);
	if (ItemIdx == INDEX_NONE) return;

	for (const FIntPoint& Cell : Items[ItemIdx].OccupiedCells)
	{
		int32 GridIdx = Cell.Y * GridWidth + Cell.X;
		ItemGrid[GridIdx].bOccupied = false;
		ItemGrid[GridIdx].ItemIndex = INDEX_NONE;
	}
	Items.RemoveAt(ItemIdx);

	// 인덱스 재정렬
	for (int32 i = 0; i < Items.Num(); i++)
	{
		for (const FIntPoint& Cell : Items[i].OccupiedCells)
		{
			int32 GridIdx = Cell.Y * GridWidth + Cell.X;
			ItemGrid[GridIdx].ItemIndex = i;
		}
	}

	OnInventoryChanged.Broadcast();
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

int32 UZS_InventoryComponent::FindItemIndexByOrigin(FIntPoint Origin) const
{
	for (int32 i = 0; i < Items.Num(); i++)
	{
		if (Items[i].Origin == Origin)
			return i;
	}
	return INDEX_NONE;
}

