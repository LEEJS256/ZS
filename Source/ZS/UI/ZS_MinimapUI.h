// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ZS_MinimapUI.generated.h"


class UCanvasPanel;
class UZS_MinimapIcon;
class UZS_MinimapIcon_Component;
class USceneCaptureComponent2D;

UCLASS()
class ZS_API UZS_MinimapUI : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* IconCanvas;

	// 동적으로 생성할 기본 아이콘 위젯 클래스 (WBP_MinimapIcon)
	UPROPERTY(EditDefaultsOnly, Category = "Minimap")
	TSubclassOf<UZS_MinimapIcon> DefaultIconClass;

	// 미니맵 위젯의 실제 픽셀 반지름 (원형/사각형 미니맵 기준 UI 반경)
	UPROPERTY(EditDefaultsOnly, Category = "Minimap")
	float MinimapWidgetRadius = 100.f;

	// 씬 캡처 컴포넌트를 가진 플레이어/카메라 추적
	UPROPERTY(BlueprintReadWrite, Category = "Minimap")
	TWeakObjectPtr<USceneCaptureComponent2D> TargetCaptureComp;

	// 현재 캔버스에 떠 있는 마커 컴포넌트 - 아이콘 위젯 매핑
	UPROPERTY()
	TMap<TWeakObjectPtr<UZS_MinimapIcon_Component>, TObjectPtr<UZS_MinimapIcon>> ActiveIconMap;

protected:
	UFUNCTION()
	void HandleIconRegistered(UZS_MinimapIcon_Component* IconComp);

	UFUNCTION()
	void HandleIconUnregistered(UZS_MinimapIcon_Component* IconComp);

	// 위젯 생성 및 캔버스 등록 헬퍼
	UZS_MinimapIcon* CreateIconWidgetForComponent(UZS_MinimapIcon_Component* IconComp);

public:
	UFUNCTION(BlueprintCallable, Category = "Minimap")
	void SetTargetCaptureComponent(USceneCaptureComponent2D* InCaptureComp);

	UFUNCTION(BlueprintPure, Category = "Minimap")
	bool WorldToMinimap(
		const FVector& WorldLocation,
		USceneCaptureComponent2D* CaptureComp,
		bool bClampToBorder,
		FVector2D& OutMinimapPos
	) const;
};
