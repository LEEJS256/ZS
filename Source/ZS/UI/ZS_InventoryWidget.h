// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ZS_InventoryWidget.generated.h"

/**
 * 
 */
class UZS_InventoryWidget;
class UZS_ItemData;
class UZS_InventoryComponent;
class UCanvasPanel;
class UZS_ItemWidget;
class UZS_ItemDragDrop;
UCLASS()
class ZS_API UZS_InventoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	//BP에서 생성시 1회만 실행
	void NativeConstruct() override;

	//에디터 프리뷰용임
	void NativePreConstruct() override;

	void SetInventory(UZS_InventoryComponent* InInventory);

	void RefreshGrid();
	
protected:
	UPROPERTY()
	TObjectPtr<UZS_InventoryComponent> InventoryRef;

	UPROPERTY(meta = (BindWidget))
	class UUniformGridPanel* GridPanel;

  	UPROPERTY(meta = (BindWidget))
    class UCanvasPanel* ItemLayer;

	UPROPERTY(EditAnywhere, Category="Inventory")
	TSubclassOf<class UZS_ItemWidget> ItemWidgetClass;  

	UPROPERTY(EditAnywhere, Category="Inventory")
	float CellSize = 128.f;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UZS_InventorySlot> SlotClass;

#pragma region Drag

	// 드래그 오버 중 슬롯 위치로 하이라이트
	virtual bool NativeOnDragOver(const FGeometry& InGeometry,
								  const FDragDropEvent& InDragDropEvent,
								  UDragDropOperation* InOperation) override;

	// 드롭 처리 → 아이템 이동
	virtual bool NativeOnDrop(const FGeometry& InGeometry,
							  const FDragDropEvent& InDragDropEvent,
							  UDragDropOperation* InOperation) override;

	// 드래그가 위젯 밖으로 나갈 때 하이라이트 초기화
	virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent,
								   UDragDropOperation* InOperation) override;

	// 드래그 취소 (ESC 등) 시 원래 위치 복원 + 투명도 복구
	virtual void NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent,
									   UDragDropOperation* InOperation) override;

	// // R키 : 드래그 중 아이템 회전
	// virtual FReply NativeOnKeyDown(const FGeometry& InGeometry,
	// 							   const FKeyEvent& InKeyEvent) override;

	// ─── 내부 유틸 ────────────────────────────────────────────

	// 마우스 절대 좌표 → 그리드 셀 인덱스 변환
	FIntPoint GetCellFromAbsolutePosition(const FGeometry& InGeometry,
										  FVector2D AbsMousePos,
										  FVector2D DragOffset) const;

	// 슬롯 하이라이트 갱신 (배치 가능: 초록, 불가: 빨강, 리셋: 흰색)
	void HighlightSlots(FIntPoint Origin, UZS_ItemData* ItemData, bool bCanPlace);
	void ResetHighlights();

	// 현재 하이라이트된 슬롯 목록 (리셋용)
	TArray<FIntPoint> HighlightedCells;

	// 드래그 중인 오퍼레이션 캐시 (R키 회전용)
	UPROPERTY()
	TObjectPtr<UZS_ItemDragDrop> ActiveDragOp;
#pragma endregion 
};
