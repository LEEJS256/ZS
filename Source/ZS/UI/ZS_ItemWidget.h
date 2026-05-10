// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ZS_ItemWidget.generated.h"

/**
 * 
 */
class UZS_ItemData;
class UImage;
class USizeBox;
class UTextBlock;
UCLASS()
class ZS_API UZS_ItemWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetItemData(UZS_ItemData* InItemData);

	// 인벤토리 내 Origin 위치 설정 (드래그 복원용)
	void SetOrigin(FIntPoint InOrigin);

	void SetCellSize(float InCellSize);

	void SetCount(int32 InCount);
protected:

	void NativeConstruct() override;
	
	// BP에서 SizeBox > Image 구조로 만들면 됨
	UPROPERTY(meta = (BindWidget))
	USizeBox* RootSizeBox;

	UPROPERTY(meta = (BindWidget))
	UImage* ItemIcon;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* CountText;

	UPROPERTY()
	int32 CachedCount = 1;
	
	UPROPERTY()
	TObjectPtr<UZS_ItemData> ItemData;
	// 인벤토리 그리드 상 위치
	UPROPERTY()
	FIntPoint Origin;

	UPROPERTY()
	float CellSize = 128.f;
	
	// ─── 드래그 앤 드롭 ───────────────────────────────────────

	// 마우스 버튼 누름 → 드래그 감지 시작
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry,
										   const FPointerEvent& InMouseEvent) override;

	// 드래그 실제 시작 시 DragDropOperation 생성
	virtual void NativeOnDragDetected(const FGeometry& InGeometry,
									  const FPointerEvent& InMouseEvent,
									  UDragDropOperation*& OutOperation) override;
};
	

