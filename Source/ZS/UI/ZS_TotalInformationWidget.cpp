// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ZS_TotalInformationWidget.h"

#include "ZS_InventoryWidget.h"
#include "ZS_StatusWidget.h"
#include "Character/ZSPlayerCharacter.h"
#include "PlayerState/ZSPlayerState.h"


void UZS_TotalInformationWidget::Update_TotalWidget()
{
	APlayerController* PC = GetOwningPlayer();
	if (!PC)
		return;

	AZSPlayerState* PS = PC->GetPlayerState<AZSPlayerState>();
	if (!PS)
		return;
	
	Status_Widget->UpdateStatus(PS->GetCurrentStatData());

	Inventory_Widget->RefreshGrid();
}

void UZS_TotalInformationWidget::NativeConstruct()
{
	Super::NativeConstruct();

	AZSPlayerCharacter* Player = Cast<AZSPlayerCharacter>(GetOwningPlayerPawn());

	if (!Player)
		return;
	
	if (Inventory_Widget)
	{
		Inventory_Widget->SetInventory(Player->Inventory);
		// 초기화
		Inventory_Widget->RefreshGrid();
	}

	if (Equipment_Widget)
	{
        
	}

	if (Status_Widget)
	{
        
	}

}

void UZS_TotalInformationWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (Inventory_Widget)
	{
		// 초기화
		Inventory_Widget->RefreshGrid();
	}
}
