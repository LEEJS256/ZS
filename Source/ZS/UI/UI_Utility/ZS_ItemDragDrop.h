// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "ZS_ItemDragDrop.generated.h"

/**
 * 
 */
class UZS_ItemWidget;
class UZS_ItemData;

UCLASS()
class ZS_API UZS_ItemDragDrop : public UDragDropOperation
{
	GENERATED_BODY()
// 데이터를 운반하는 컨테이너 역할
public:
	// 드래그 중인 아이템 데이터
	UPROPERTY()
	TObjectPtr<UZS_ItemData> ItemData;

	// 인벤토리 내 원래 Origin 위치
	UPROPERTY()
	FIntPoint OriginalOrigin;

	// 드래그 시작 시 아이템 내에서 클릭된 셀 오프셋 (픽셀 단위)
	UPROPERTY()
	FVector2D DragOffset;


	
};
