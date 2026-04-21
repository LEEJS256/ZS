// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ZS_TotalInformationWidget.generated.h"

/**
 * 
 */

class UZS_InventoryWidget;
class UZS_EquipmentWIdget;
class UZS_StatusWidget;
UCLASS()
class ZS_API UZS_TotalInformationWidget : public UUserWidget
{
	GENERATED_BODY()
public:
//	UZS_TotalInformationWidget();

protected:

	virtual void NativeConstruct() override;
	
#pragma region UI
	UPROPERTY(meta = (BindWidget))
	UZS_InventoryWidget* Inventory_Widget;

	UPROPERTY(meta = (BindWidget))
	UZS_EquipmentWIdget* Equipment_Widget;

	UPROPERTY(meta = (BindWidget))
	UZS_StatusWidget* Status_Widget;
	
	// UPROPERTY()
	// UZS_InventoryWidget* Inventory_Widget;
	//
	// UPROPERTY(EditAnywhere, Category="UI")
	// TSubclassOf<UZS_InventoryWidget> Inventory_WidgetClass;
	//
	// UPROPERTY()
	// UZS_EquipmentWIdget* Equipment_Widget;
	//
	// UPROPERTY(EditAnywhere, Category="UI")
	// TSubclassOf<UZS_EquipmentWIdget> Equipment_WidgetClass;
	//
	// UPROPERTY()
	// UZS_StatusWidget* Status_Widget;
	//
	// UPROPERTY(EditAnywhere, Category="UI")
	// TSubclassOf<UZS_StatusWidget> Status_WidgetClass;

	
#pragma  endregion 
};
