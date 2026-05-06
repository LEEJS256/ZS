// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ZS_InventoryComponent.generated.h"

class UZS_ItemData;

#pragma  region Struct

USTRUCT(BlueprintType)
struct FInventoryItem_Component
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<UZS_ItemData> ItemData;

	UPROPERTY()
	int32 Count = 1;

	UPROPERTY()
	FIntPoint Origin;

	UPROPERTY()
	TArray<FIntPoint> OccupiedCells;
};

USTRUCT()
struct FGridCell_Component
{
	GENERATED_BODY()

	bool bOccupied = false;

	UPROPERTY()
	int32 ItemIndex = INDEX_NONE;
};

#pragma  endregion 


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryChanged);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ZS_API UZS_InventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UZS_InventoryComponent();

	bool AddItem(UZS_ItemData* ItemData);
	bool PlaceItem(UZS_ItemData* ItemData, FIntPoint Origin);
	bool CanPlaceItem(UZS_ItemData* ItemData, FIntPoint Origin);

	void ClearGrid();

	// Getter
	const TArray<FInventoryItem_Component>& GetItems() const { return Items; }
	const TArray<FGridCell_Component>& GetGrid() const { return ItemGrid; }
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(BlueprintAssignable)
	FOnInventoryChanged OnInventoryChanged;

private:
	UPROPERTY()
	TArray<FInventoryItem_Component> Items;

	UPROPERTY()
	TArray<FGridCell_Component> ItemGrid;

public:

	UPROPERTY(EditAnywhere, Category="Inventory")
	int32 GridWidth = 5;

	UPROPERTY(EditAnywhere, Category="Inventory")
	int32 GridHeight = 8;
		
};
